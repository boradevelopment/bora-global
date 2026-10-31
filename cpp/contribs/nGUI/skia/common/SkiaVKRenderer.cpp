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

std::vector<SkiaVKRenderer*> SkiaVKRenderer::activeRenderers;
std::mutex SkiaVKRenderer::globalMutex;

SkiaVKRenderer::SkiaVKRenderer(sk_sp<GrDirectContext> context, bnGraphicsVK *engine)
    : context(std::move(context)), engine(engine)
{
    std::lock_guard<std::mutex> lock(globalMutex);
    activeRenderers.push_back(this);
}

SkiaVKRenderer::~SkiaVKRenderer()
{
    std::lock_guard<std::mutex> lock(globalMutex);
    auto it = std::remove(activeRenderers.begin(), activeRenderers.end(), this);
    activeRenderers.erase(it, activeRenderers.end());
}

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

IGUISurface* SkiaVKRenderer::createFrameSurface(const UXRect& rect, ResourceHandle<ITexture> texture) {
    TextureVK* backBuffer = texture.Exists() ? (TextureVK*)texture.Get() : (TextureVK*)engine->GetSwapchainImage();
    if (!backBuffer) return nullptr;
    VulkanSurface vkSurface;
    vkSurface.buffer = backBuffer;
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
    for (auto& cachedSurface : surfaces)
    {
        auto existingRect = cachedSurface.uxSurface->GetRect();
        if (!texture && existingRect.GetWidth() >= localRect.GetWidth() &&
          existingRect.GetHeight() >= localRect.GetHeight() && backBuffer == cachedSurface.buffer) // reuse swapchain but make sure the images are the same
        {
            return cachedSurface.uxSurface.get();
        }
    }

    if (!texture)
    { // we are using backBuffer so we need to switch layouts.
        vkSurface.isSwapchain = true;
        auto copyCmd = engine->BeginSingleTimeCommands(engine->copyPool);
        copyCmd->PipelineBarrier(backBuffer, ImageLayout::RenderTarget, ImageAccessLayout::Write, ImageAccessLayout::Write);
        engine->EndSingleTimeCommands(copyCmd);
    }

    GrBackendFormat backendFormat = GrBackendFormats::MakeVk(ToVkFormat(backBuffer->desc.format));
    GrVkImageInfo vkImageInfo = {};
    if (vkSurface.uxSurface)
    {
        vkImageInfo.fAlloc = skgpu::VulkanAlloc();
    } else
    {
        VkMemoryRequirements memReqs;
        vkGetImageMemoryRequirements(engine->device, backBuffer->image, &memReqs);
        vkImageInfo.fAlloc.fMemory = backBuffer->memory;
        vkImageInfo.fAlloc.fOffset = 0;
        vkImageInfo.fAlloc.fSize = memReqs.size;
        vkImageInfo.fAlloc.fFlags = 0;
    }
    vkImageInfo.fImage               = backBuffer->image;
    vkImageInfo.fImageTiling         = VK_IMAGE_TILING_OPTIMAL;
    vkImageInfo.fImageLayout        = ToVkImageLayout(backBuffer->explicitLayout);
    vkImageInfo.fFormat              = ToVkFormat(backBuffer->desc.format);
    vkImageInfo.fImageUsageFlags     = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    vkImageInfo.fSampleCount         = 1;
    vkImageInfo.fLevelCount          = 1;
    vkImageInfo.fCurrentQueueFamily   = engine->familyIndices.graphicsFamily;
    vkImageInfo.fProtected           = GrProtected::kNo;
    // check if engine families are different!!!
    vkImageInfo.fSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    vkImageInfo.fYcbcrConversionInfo = {};

    auto backendRT = GrBackendRenderTargets::MakeVk(backBuffer->desc.width, backBuffer->desc.height, vkImageInfo);

    SkSurfaceProps props(0, kUnknown_SkPixelGeometry);
    sk_sp<SkColorSpace> colorSpace = SkColorSpace::MakeSRGB();

    auto skSurface = SkSurfaces::WrapBackendRenderTarget(
    context.get(),
    backendRT,
    kTopLeft_GrSurfaceOrigin,
    kBGRA_8888_SkColorType,
    colorSpace,
    &props
    );

    if (!skSurface) return nullptr;
    vkSurface.uxSurface = std::make_unique<SkiaSurface>(rect, skSurface);

    if (vkSurface.uxSurface && vkSurface.uxSurface->GetNativeHandle()) {
        if (auto* canvas = vkSurface.uxSurface->GetCanvas()) {
            canvas->Save();
            canvas->Translate(static_cast<float>(localRect.left), static_cast<float>(localRect.top));
            canvas->ClipRect(localRect);
        }

        IGUISurface* rawPtr = vkSurface.uxSurface.get();
        surfaces.push_back(std::move(vkSurface));
        return rawPtr;
    }

    return nullptr;
}

IGUISurface* SkiaVKRenderer::getLastFrameSurface() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (surfaces.empty()) return nullptr;

    auto* surface = surfaces.back().uxSurface.get();
    return (surface && surface->GetNativeHandle()) ? surface : nullptr;
}

void SkiaVKRenderer::endframe()
{
    std::lock_guard<std::mutex> lock(globalMutex);
    bool swapchainUsed = false;
    for (auto renderer : activeRenderers)
    {
        for (auto& surface : renderer->surfaces) {
            if (auto* canvas = surface.uxSurface->GetCanvas()) {
                canvas->Restore();
            }
            swapchainUsed = surface.isSwapchain;
        }
    }

    if (context) {
        context->flushAndSubmit();
    }

    if (swapchainUsed)
    {
        auto cmd = engine->BeginSingleTimeCommands(engine->copyPool);
        cmd->PipelineBarrier(
            engine->GetSwapchainImage(),
            ImageLayout::Present,
            ImageAccessLayout::Present,
            ImageAccessLayout::Present
        );
        engine->EndSingleTimeCommands(cmd);
    }
}

// void SkiaVKRenderer::draw() {
//
//     // Clear active frame list (cached persistent surfaces remain alive in m_surfaceCache)
//     // surfaces.clear();
// }

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

    if (context) {
        context->flushAndSubmit(GrSyncCpu::kYes);
        context->purgeUnlockedResources(GrPurgeResourceOptions::kAllResources);
    }
}
#endif