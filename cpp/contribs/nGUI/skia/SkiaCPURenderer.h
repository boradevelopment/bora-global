// This header is apart of the BORA Source
// Check LICENSE.md for more information regarding the BORA license.
/* 
 * FileName: SkiaCPURenderer.h
 * Purpose: A CPU Renderer for SKIA, this works for all platforms.
 */

#pragma once
#include "interfaces/IGUIRenderer.h"
#include "interfaces/GUISkiaSysInfo.h"
#ifdef BORA_UI_SUPPORT
class SkiaCPURenderer : public IGUIRenderer {
public:
    IGUISurface* beginFrame(int width, int height) override;
    void endFrame() override;

private:
    sk_sp<SkSurface> surface;
};
#endif