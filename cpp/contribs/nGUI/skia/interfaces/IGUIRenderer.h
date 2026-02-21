// This header is apart of the BORA Source
// Check LICENSE.md for more information regarding the BORA license.

/* 
 * FileName: IGUIRenderer.h
 * Purpose: Acts as a interface for GUI Renderable classes for SKIA.
 */
#pragma once
#include "interfaces/GUISkiaSysInfo.h"

class IGUIRenderer {
public:
    virtual ~IGUIRenderer() = default;

    virtual SkCanvas* beginFrame(int width, int height) = 0;
    virtual void endFrame() = 0;
};
