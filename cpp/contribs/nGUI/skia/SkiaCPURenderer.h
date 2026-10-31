// This header is apart of the BORA Source
// Check LICENSE.md for more information regarding the BORA license.
/* 
 * FileName: SkiaCPURenderer.h
 * Purpose: A CPU Renderer for SKIA, this works for all platforms.
 *          This has two different modes, Basic rasterization for surfaces which doesn't render anything but supplies surfaces.
 *          Window mode renders the surfaces onto the screen regardless of compatibility with SKIA's GPU.
 *
 *          Window mode is useful for libraries like DX11 which have no support for Skia's backend but you
 *          as a developer don't want to add extra code to manage rendering of the surface textures.
 */
#pragma once
#include "interfaces/IGUIRenderer.h"
#include "interfaces/GUISkiaSysInfo.h"
#include "nGraphics/GraphicsAbstractions.h"
#include "nGraphics/ExplicitGraphicsAbstract.h"
#include "nGraphics/ImmediateGraphicsAbstract.h"

#ifdef BORA_UI_SUPPORT
class bnWindow;
class IGraphicsDevice;

struct CPUSurfaceData
{
    IGUISurface* uxSurface = nullptr;
    ResourceHandle<IDescriptorSet> set;
    ResourceHandle<ITexture>* texture = nullptr;
    ResourceHandle<IBuffer>* vertexBuffer = nullptr;
    ResourceHandle<IPipeline>* pipeline = nullptr;
    ResourceHandle<IDescriptorPool>* dPool = nullptr;
    ViewPort viewport;
    ResourceHandle<IViewPort> viewportHandle;
    bool isDirty = true;         // Only update GPU texture if the Skia surface changed
    bool isGeometryDirty = true; // Only update buffer if geometry changed (rect)
};

class SkiaCPURenderer : public IGUIRenderer {
public:
    ~SkiaCPURenderer() override;
    SkiaCPURenderer();
    SkiaCPURenderer(bnWindow* window); // CPU based rendering which renders on the GPU if not supported.

    sk_sp<SkSurface> CreateSurfaceFromTexture(ITexture* texture);
    void Flush();
    IGUISurface* createFrameSurface(const UXRect& rect, ResourceHandle<ITexture> texture) override;
    IGUISurface* createFrameSurface(ResourceHandle<ITexture> texture) override;
    IGUISurface* getLastFrameSurface() override;
    bool isCPU() override
    {
        return true;
    };
    void finalizeResources() override;
    void OnResize() override {};
    void endframe() override;

private:

    void destroySurface(CPUSurfaceData& cpuSurface);
    bnWindow* window = nullptr;
    IGraphicsDevice* engine = nullptr;
    std::vector<CPUSurfaceData> surfaces;
    std::vector<CPUSurfaceData> lastFrameSurfaces;

    // CPU based rendering
    ResourceHandle<IShader> fragmentShader;
    ResourceHandle<IShader> vertexShader;
    ResourceHandle<IDescriptorSetLayout> dSetLayout;
    ResourceHandle<IInputLayout> inputLayout;
    ResourceHandle<ISamplerState> samplerState;
    ResourceHandle<IBlendState> blending;
    ResourceHandle<IRasterizerState> rast;
    ResourceHandle<IDepthStencilState> depth;
    // sk_sp<SkSurface> surface;
};
#endif