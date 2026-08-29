// This header is apart of the BORA Source
// Check LICENSE.md for more information regarding the BORA license.
/* 
 * FileName: SkiaD3D12Renderer.h
 * Purpose: A GPU Renderer for VK for GUI rendering!
 */
#pragma once
#if defined(BORA_UI_SUPPORT) && defined(BORA_HAS_VULKAN)
#include "../interfaces/IGUIRenderer.h"
#include "../interfaces/GUISkiaSysInfo.h"
#include "nGraphics/GraphicsAbstractions.h"
#include "nGUI/skia/abstractions/SkiaCanvas.h"
#include "nGUI/skia/abstractions/SkiaSurface.h"
#include <mutex>

class bnGraphicsVK;

class SkiaVKRenderer : public IGUIRenderer {
public:
    SkiaVKRenderer(sk_sp<GrDirectContext> context, bnGraphicsVK* engine);
    sk_sp<SkSurface> CreateSurfaceFromTexture(ITexture* texture);
    void Flush();
    IGUISurface* createFrameSurface(const WindowRect& rect, ResourceHandle<ITexture> texture) override;
    IGUISurface* createFrameSurface(ResourceHandle<ITexture> texture) override;
    IGUISurface* getLastFrameSurface() override;
    void draw() override;
    void finalizeResources() override;
    void OnResize() override;


private:
    std::unordered_map<ITexture*, std::unique_ptr<IGUISurface>> m_surfaceCache;
    std::vector<IGUISurface*> surfaces;
    sk_sp<GrDirectContext> context;
    std::mutex m_mutex;
    bnGraphicsVK* engine;
};
#endif