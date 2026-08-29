#include <include/core/SkColorSpace.h>
#include <include/gpu/ganesh/GrBackendSurface.h>
#include "SkiaMTLRenderer.h"
#include "nGraphics/bnGraphicsMTL.h"

#if defined(BORA_UI_SUPPORT) && defined(__APPLE__)
SkiaMTLRenderer::SkiaMTLRenderer(sk_sp<GrDirectContext> context, bnGraphicsMTL *engine) : context(context), engine(engine) {

}

sk_sp<SkSurface> SkiaMTLRenderer::CreateSurfaceFromTexture(ITexture *texture) {
    GrMtlTextureInfo info = {};
    info.fTexture = sk_ret_cfp((const void*)texture->GetNativeHandle());

    GrBackendTexture backend = GrBackendTextures::MakeMtl(texture->desc.width, texture->desc.height, skgpu::Mipmapped::kNo, info);
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

void SkiaMTLRenderer::Flush() {

}

SkCanvas *SkiaMTLRenderer::beginFrame(int width, int height) {
    return nullptr;
}

void SkiaMTLRenderer::endFrame() {

}

void SkiaMTLRenderer::DestroyFrameResources() {

}

void SkiaMTLRenderer::Clear() {
    SkPaint paint;
    paint.setColor(SK_ColorGREEN);
    paint.setAntiAlias(true);       // smooth edges

    // Draw a circle at center
    float cx = 1080 * 0.5f;
    float cy = 1920 * 0.5f;
    float radius = 100.0f;
    surface->getCanvas()->drawCircle(cx, cy, radius, paint);
}

void SkiaMTLRenderer::PrepareForResize() {

}

#endif