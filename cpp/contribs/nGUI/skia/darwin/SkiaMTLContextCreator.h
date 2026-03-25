#pragma once
#if defined(BORA_UI_SUPPORT) && defined(__APPLE__)
#import <nWindow/bnWindow.h>
#include "nGUI/skia/interfaces/GUISkiaSysInfo.h"

class SkiaMTLContextCreator {
public:
    static sk_sp<GaneshGPUContextType> createMTLContext(void* device, void* queue);
    static sk_sp<GaneshGPUContextType> createNewContextFromWindow(const bnWindow* window);
};
#endif