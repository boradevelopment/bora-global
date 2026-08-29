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
#include "nWindow/bnWindow.h"
#include "skia/GPUContextManager.h"
#include "skia/SkiaCPURenderer.h"
#include "skia/common/SkiaVKRenderer.h"
#include "skia/interfaces/IGUIRenderer.h"

class GUIRoot {
public:
    GUIRoot(bnWindow* window) : window(window)
    {

    };

    static IGUIRenderer* createCPURenderer()
    {
        return new SkiaCPURenderer();
        // renderer = std::make_unique<SkiaCPURenderer>();
    }

    IGUIRenderer* createGPURenderer();

private:
    bnWindow* window;
};
#endif