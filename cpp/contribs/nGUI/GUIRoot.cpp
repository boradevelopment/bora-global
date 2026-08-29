// Apart of the BORA Runtime Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.
#ifdef BORA_UI_SUPPORT
#include "GUIRoot.h"


IGUIRenderer* GUIRoot::createGPURenderer()
{
    if (window->choice == VULKAN)
    {
        return new SkiaVKRenderer(GPUContextManager::getGaneshContextForWindow(window), (bnGraphicsVK*)window->rootDevice);
    }
#ifdef WIN32
    if (window->choice == D3D12)
    {
        return new
    }
#endif

    return nullptr;
}

#endif