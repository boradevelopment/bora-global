// This header is apart of the BORA Source
// Check LICENSE.md for more information regarding the BORA license.
/* 
 * FileName: GUIRoot.h
 * Purpose: Root of GUI base classes
 */
#pragma once
#ifdef BORA_UI_SUPPORT
#include <memory>
#include "nGraphics/bnGraphicsVK.h"
// #include "nWindow/bnWindow.h"
#include "skia/GPUContextManager.h"
#include "skia/SkiaCPURenderer.h"
#include "skia/common/SkiaVKRenderer.h"
#include "skia/interfaces/IGUIRenderer.h"

class bnWindow;

class GUIRoot {
public:
    GUIRoot(bnWindow* window) : window(window)
    {
    };

    // Creates a basic CPU renderer, all managed on the CPU.
    static IGUIRenderer* createCPURenderer()
    {
        return new SkiaCPURenderer();
    }

    IGUIRenderer* createGPURenderer();


private:
    bnWindow* window;
    std::shared_ptr<IGUIRenderer> defaultCPURenderer = nullptr;
    std::vector<std::shared_ptr<IGUIRenderer>> windowRenderers;
};
#endif