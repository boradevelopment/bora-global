// This header is apart of the BORA Source
// Check LICENSE.md for more information regarding the BORA license.
/* 
 * FileName: SkiaD3D12Renderer.h
 * Purpose: A GPU Renderer for D3D12 for GUI rendering!
 */
#pragma once
#if defined(BORA_UI_SUPPORT) && defined(WIN32)
#include "../interfaces/IGUIRenderer.h"
#include "../interfaces/GUISkiaSysInfo.h"
#include "nGraphics/GraphicsAbstractions.h"

class bnGraphicsD3D12;

class SkiaD3D12Renderer : public IGUIRenderer {
public:
    SkiaD3D12Renderer(sk_sp<GrDirectContext> context, bnGraphicsD3D12* engine);

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
    bnGraphicsD3D12* engine;
};
#endif