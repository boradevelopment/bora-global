// This header is apart of the BORA Source
// Check LICENSE.md for more information regarding the BORA license.

/* 
 * FileName: IGUIRenderer.h
 * Purpose: Acts as a interface for GUI Renderable classes for SKIA.
 */
#ifdef BORA_UI_SUPPORT
#pragma once
#include <include/core/SkCanvas.h>
#include "../interfaces/GUISkiaSysInfo.h"

class IGUIRenderer {
public:
    virtual ~IGUIRenderer() = default;

    virtual SkCanvas* beginFrame(int width, int height) = 0;
    virtual void endFrame() = 0;
};
#endif