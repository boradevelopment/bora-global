#pragma once
#if defined(BORA_UI_SUPPORT) && defined(__APPLE__)
#include "../interfaces/IGUIRenderer.h"
#include "../interfaces/GUISkiaSysInfo.h"
#include "nGraphics/GraphicsAbstractions.h"
class bnGraphicsMTL;

class SkiaMTLRenderer : public IGUIRenderer {
public:
    SkiaMTLRenderer(sk_sp<GrDirectContext> context, bnGraphicsMTL* engine);

    sk_sp<SkSurface> CreateSurfaceFromTexture(ITexture* texture);
    void Flush();
    SkCanvas* beginFrame(int width, int height) override;
    void endFrame() override;
    void DestroyFrameResources();
    void Clear();
    void PrepareForResize();

private:
    sk_sp<GrDirectContext> context;
    sk_sp<SkSurface> surface;
    bnGraphicsMTL* engine;
};
#endif