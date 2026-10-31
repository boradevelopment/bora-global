// Apart of the BORA Runtime Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.
#ifdef BORA_UI_SUPPORT
#include "GUIRoot.h"
#include "nWindow/bnWindow.h"

IGUIRenderer* GUIRoot::createGPURenderer()
{
    if (window->choice == VULKAN)
    {
        windowRenderers.push_back(std::make_shared<SkiaVKRenderer>(GPUContextManager::getGaneshContextForWindow(window), (bnGraphicsVK*)window->rootDevice));
        return windowRenderers.back().get();
    }
#ifdef WIN32
    if (window->choice == D3D12)
    {
        return new
    }
#endif
    // backwards-compatability - uses the Window mode CPU renderer if GPU doesn't support Skia backends.
    if (!defaultCPURenderer)
    {
        defaultCPURenderer = std::make_shared<SkiaCPURenderer>(window);
    }
    return defaultCPURenderer.get();
}

#endif