// Apart of the BORA Runtime Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.
#ifdef BORA_UI_SUPPORT
#include "SkiaD3D12Renderer.h"
#include <include/core/SkColorSpace.h>
#include <include/gpu/ganesh/GrBackendSurface.h>
#include "nGraphics/bnGraphicsD3D12.h"

SkiaD3D12Renderer::SkiaD3D12Renderer(sk_sp<GrDirectContext> context, bnGraphicsD3D12 *engine) : context(context), engine(engine) {
}

sk_sp<SkSurface> SkiaD3D12Renderer::CreateSurfaceFromTexture(ITexture *texture) {
    auto res = (ID3D12Resource *) texture->GetNativeHandle();
    // res->AddRef();

    GrD3DTextureResourceInfo info = {};
    info.fResource.retain(res);
    info.fResourceState = ToD3D12ResourceState(texture->explicitLayout, ImageAccessLayout::None);
    info.fFormat = ToDXGIFormat(texture->desc.format);
    info.fLevelCount = 1;
    info.fSampleQualityPattern = 0;
    info.fSampleCount = 1;
    info.fProtected = GrProtected::kNo;

    GrBackendTexture backend(texture->desc.width, texture->desc.height, info);
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

void SkiaD3D12Renderer::Flush() {
    context->flushAndSubmit(GrSyncCpu::kYes);
}

SkCanvas* SkiaD3D12Renderer::beginFrame(int width, int height) {
    ITexture* backBuffer = engine->GetSwapchainImage(); // questionable swapchain
    auto res = (ID3D12Resource *) backBuffer->GetNativeHandle();
    res->AddRef();

    GrD3DTextureResourceInfo info = {};
    info.fResource = gr_cp(res);
    info.fResourceState = D3D12_RESOURCE_STATE_PRESENT;
    info.fFormat = ToDXGIFormat(backBuffer->desc.format);
    info.fLevelCount = 1;
    info.fSampleQualityPattern = 0; //DXGI_STANDARD_MULTISAMPLE_QUALITY_PATTERN
    info.fSampleCount = engine->config.msaaSamples > 0 ? 1 : 0;
    info.fProtected = GrProtected::kNo;

    GrBackendRenderTarget backendRT(width, height, info);
    SkSurfaceProps props(0, kUnknown_SkPixelGeometry);
    sk_sp<SkColorSpace> colorSpace = SkColorSpace::MakeSRGB(); // todo: check
    surface = SkSurfaces::WrapBackendRenderTarget(
        context.get(),
        backendRT,
        kTopLeft_GrSurfaceOrigin,
        kRGBA_8888_SkColorType,
        colorSpace,
        &props,
        nullptr,
        nullptr
    );

    if (surface)
        return surface->getCanvas();
    else
        return nullptr;

}

void SkiaD3D12Renderer::endFrame() {
    context->flushAndSubmit();
    auto cmd = engine->BeginSingleTimeCommands(engine->copyPool);
    cmd->PipelineBarrier(
     engine->GetSwapchainImage(),
     ImageLayout::RenderTarget,
     ImageLayout::Present,
     ImageAccessLayout::Present,
     ImageAccessLayout::Present
        );
    engine->EndSingleTimeCommands(cmd);
}

void SkiaD3D12Renderer::DestroyFrameResources() {

}

void SkiaD3D12Renderer::Clear() {
    // surface->getCanvas()->clear(SK_ColorBLACK);

    SkPaint paint;
    paint.setColor(SK_ColorMAGENTA);    // solid red
    paint.setAntiAlias(true);       // smooth edges

    // Draw a circle at center
    float cx = 1080 * 0.5f;
    float cy = 1920 * 0.5f;
    float radius = 100.0f;
    surface->getCanvas()->drawCircle(cx, cy, radius, paint);
}

void SkiaD3D12Renderer::PrepareForResize() {
    if (!surface) return;
    surface.reset();
    context->flushAndSubmit(GrSyncCpu::kYes);
    context->purgeUnlockedResources(GrPurgeResourceOptions::kAllResources);
}
#endif