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

struct VulkanSurface
{
    std::unique_ptr<IGUISurface> uxSurface = nullptr;
    ITexture* buffer;
    bool isSwapchain = false;

    bool operator==(const VulkanSurface& other) const {
        return uxSurface == other.uxSurface && buffer == other.buffer;
    }
};

namespace std {
    template <>
    struct hash<VulkanSurface> {
        size_t operator()(const VulkanSurface& s) const {
            size_t h1 = std::hash<void*>{}(static_cast<void*>(s.uxSurface.get()));
            size_t h2 = std::hash<void*>{}(static_cast<void*>(s.buffer));
            return h1 ^ (h2 << 1);
        }
    };
}

class SkiaVKRenderer : public IGUIRenderer {
public:
    SkiaVKRenderer(sk_sp<GrDirectContext> context, bnGraphicsVK* engine);
    ~SkiaVKRenderer();
    sk_sp<SkSurface> CreateSurfaceFromTexture(ITexture* texture);
    void Flush();
    IGUISurface* createFrameSurface(const UXRect& rect, ResourceHandle<ITexture> texture) override;
    IGUISurface* createFrameSurface(ResourceHandle<ITexture> texture) override;
    IGUISurface* getLastFrameSurface() override;
    bool isCPU() override
    {
        return false;
    };
    void endframe() override;
    void finalizeResources() override {};
    void OnResize() override;


private:
    std::vector<VulkanSurface> surfaces;
    std::mutex m_mutex;

    // Static global registry shared across all renderer instances
    static std::vector<SkiaVKRenderer*> activeRenderers;
    static std::mutex globalMutex;

    sk_sp<GrDirectContext> context;
    bnGraphicsVK* engine;
};
#endif