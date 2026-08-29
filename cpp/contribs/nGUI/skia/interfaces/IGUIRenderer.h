// This header is apart of the BORA Source
// Check LICENSE.md for more information regarding the BORA license.

/* 
 * FileName: IGUIRenderer.h
 * Purpose: Acts as a interface for GUI Renderable classes for SKIA.
 */
#ifdef BORA_UI_SUPPORT
#pragma once
#include "IGUICanvas.h"
#include "IGUISurface.h"
#include "../interfaces/GUISkiaSysInfo.h"
#include "nCommon/Resources.h"
#include "nWindow/bnWindowAbstracts.h"

class ITexture;

class IGUIRenderer {
public:
    virtual ~IGUIRenderer() = default;

    virtual IGUISurface* createFrameSurface(const WindowRect& rect, ResourceHandle<ITexture> texture = nullptr) = 0;
    virtual IGUISurface* createFrameSurface(ResourceHandle<ITexture> texture = nullptr) = 0;
    virtual IGUISurface* getLastFrameSurface() = 0;
    virtual void draw() = 0;
    virtual void finalizeResources() = 0;
    virtual void OnResize();

};
#endif