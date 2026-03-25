#pragma once
#if defined(BORA_UI_SUPPORT) && defined(WIN32)
#include "nGUI/skia/interfaces/GUISkiaSysInfo.h"
class SkiaD3D12ContextCreator {
    static sk_sp<GaneshGPUContextType> createD3DContext(ID3D12Device* device,
                                                        ID3D12CommandQueue* queue,
                                                        IDXGIAdapter1* adapter);

    static sk_sp<GaneshGPUContextType> createNewContextFromWindow(bnWindow* window);
};
#endif