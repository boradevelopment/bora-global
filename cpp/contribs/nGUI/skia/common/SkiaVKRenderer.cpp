// Apart of the BORA Runtime Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.
#if defined(BORA_UI_SUPPORT) && defined(BORA_HAS_VULKAN)
#include "SkiaVKRenderer.h"
#include <include/core/SkColorSpace.h>
#include <include/gpu/ganesh/GrBackendSurface.h>
#include <include/gpu/ganesh/vk/GrVkBackendSurface.h>
#include <include/gpu/ganesh/vk/GrVkTypes.h>

#include <utility>
#include <mutex>
#include "nGraphics/bnGraphicsVK.h"

SkiaVKRenderer::SkiaVKRenderer(sk_sp<GrDirectContext> context, bnGraphicsVK *engine)
    : context(std::move(context)), engine(engine) {}

sk_sp<SkSurface> SkiaVKRenderer::CreateSurfaceFromTexture(ITexture *texture) {
    if (!texture) return nullptr;

    GrVkImageInfo vkImageInfo = {};
    vkImageInfo.fImage               = reinterpret_cast<VkImage>(texture->GetNativeHandle());
    vkImageInfo.fImageTiling         = VK_IMAGE_TILING_OPTIMAL;
    vkImageInfo.fImageLayout        = ToVkImageLayout(texture->explicitLayout);
    vkImageInfo.fFormat              = ToVkFormat(texture->desc.format);
    vkImageInfo.fImageUsageFlags     = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    vkImageInfo.fSampleCount         = 1;
    vkImageInfo.fLevelCount          = 1;
    vkImageInfo.fCurrentQueueFamily   = VK_QUEUE_FAMILY_IGNORED;
    vkImageInfo.fProtected           = GrProtected::kNo;

    auto backend = GrBackendTextures::MakeVk(texture->desc.width, texture->desc.height, vkImageInfo);

    // Thread safety note: context calls must remain serialized to the render thread
    std::lock_guard<std::mutex> lock(m_mutex);
    return SkSurfaces::WrapBackendTexture(
        context.get(),
        backend,
        kTopLeft_GrSurfaceOrigin,
        1,
        kRGBA_8888_SkColorType,
        nullptr,
        nullptr
    );
}

void SkiaVKRenderer::Flush() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (context) {
        context->flushAndSubmit(GrSyncCpu::kYes);
    }
}

IGUISurface* SkiaVKRenderer::createFrameSurface(ResourceHandle<ITexture> texture)
{
    return createFrameSurface({}, texture);
}

IGUISurface* SkiaVKRenderer::createFrameSurface(const WindowRect& rect, ResourceHandle<ITexture> texture) {
    ITexture* backBuffer = texture.Exists() ? texture.Get() : engine->GetSwapchainImage();
    if (!backBuffer) return nullptr;
    std::lock_guard<std::mutex> lock(m_mutex);
    auto localRect = rect;
    if (localRect.IsEmpty())
    {
        u32 height = backBuffer->desc.height;
        u32 width = backBuffer->desc.width;

        localRect.left   = 0;
        localRect.top    = 0;
        localRect.right  = static_cast<long>(width);
        localRect.bottom = static_cast<long>(height);
    }

    // 1. Check existing cache
    auto it = m_surfaceCache.find(backBuffer);
    if (it != m_surfaceCache.end()) {
        auto* cachedSurface = it->second.get();
        if (cachedSurface->GetWidth() == rect.GetWidth() && cachedSurface->GetHeight() == rect.GetHeight()) {
            surfaces.push_back(cachedSurface);
            return cachedSurface;
        }
        m_surfaceCache.erase(it);
    }

    // 2. Wrap Vulkan Render Target
    GrVkImageInfo vkImageInfo = {};
    vkImageInfo.fImage               = reinterpret_cast<VkImage>(backBuffer->GetNativeHandle());
    vkImageInfo.fImageTiling         = VK_IMAGE_TILING_OPTIMAL;
    vkImageInfo.fImageLayout        = ToVkImageLayout(backBuffer->explicitLayout);
    vkImageInfo.fFormat              = ToVkFormat(backBuffer->desc.format);
    vkImageInfo.fImageUsageFlags     = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    vkImageInfo.fSampleCount         = 1;
    vkImageInfo.fLevelCount          = 1;
    vkImageInfo.fCurrentQueueFamily   = VK_QUEUE_FAMILY_IGNORED;
    vkImageInfo.fProtected           = GrProtected::kNo;

    auto backendRT = GrBackendRenderTargets::MakeVk(localRect.GetWidth(), localRect.GetHeight(), vkImageInfo);
    SkSurfaceProps props(0, kUnknown_SkPixelGeometry);
    sk_sp<SkColorSpace> colorSpace = SkColorSpace::MakeSRGB();

    auto surface = std::make_unique<SkiaSurface>(SkSurfaces::WrapBackendRenderTarget(
        context.get(),
        backendRT,
        kTopLeft_GrSurfaceOrigin,
        kRGBA_8888_SkColorType,
        colorSpace,
        &props,
        nullptr,
        nullptr
    ));

    if (surface && surface->GetNativeHandle()) {
        if (auto* canvas = surface->GetCanvas()) {
            canvas->Save();
            canvas->Translate(static_cast<float>(localRect.left), static_cast<float>(localRect.top));
            canvas->ClipRect(localRect);
        }

        IGUISurface* rawPtr = surface.get();
        m_surfaceCache[backBuffer] = std::move(surface);
        surfaces.push_back(rawPtr);
        return rawPtr;
    }

    return nullptr;
}

IGUISurface* SkiaVKRenderer::getLastFrameSurface() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (surfaces.empty()) return nullptr;

    auto* surface = surfaces.back();
    return (surface && surface->GetNativeHandle()) ? surface : nullptr;
}

void SkiaVKRenderer::draw() {
    std::lock_guard<std::mutex> lock(m_mutex);

    for (IGUISurface* surface : surfaces) {
        if (auto* canvas = surface->GetCanvas()) {
            canvas->Restore();
        }
    }

    if (context) {
        context->flushAndSubmit();
    }

    // Pipeline barrier transition outside mutex if engine call manages its own sync
    auto cmd = engine->BeginSingleTimeCommands(engine->copyPool);
    cmd->PipelineBarrier(
        engine->GetSwapchainImage(),
        ImageLayout::RenderTarget,
        ImageLayout::Present,
        ImageAccessLayout::Present,
        ImageAccessLayout::Present
    );
    engine->EndSingleTimeCommands(cmd);

    // Clear active frame list (cached persistent surfaces remain alive in m_surfaceCache)
    surfaces.clear();
}

// void SkiaVKRenderer::Clear() {
//     IGUISurface* surface = getLastFrameSurface();
//     if (!surface) return;
//
//     if (auto* canvas = surface->GetCanvas()) {
//         canvas->Clear({0, 0, 0, 0});
//         GUIPaint paint;
//         paint.color = {0.24f, 0.23f, 0.32f, 1.0f};
//         paint.isAntiAlias = true;
//
//         float cx = 1080 * 0.5f;
//         float cy = 1920 * 0.5f;
//         float radius = 100.0f;
//         canvas->DrawCircle(Vec2(cx, cy), radius, paint);
//     }
// }

void SkiaVKRenderer::OnResize() {
    std::lock_guard<std::mutex> lock(m_mutex);

    surfaces.clear();
    m_surfaceCache.clear();

    if (context) {
        context->flushAndSubmit(GrSyncCpu::kYes);
        context->purgeUnlockedResources(GrPurgeResourceOptions::kAllResources);
    }
}
#endif