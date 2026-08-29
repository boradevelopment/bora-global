// This header is apart of the BORA Source
// Check LICENSE.md for more information regarding the BORA license.

/* 
 * FileName: GPUContextManager.h
 * Purpose: ?
 */
#pragma once
#ifdef BORA_UI_SUPPORT
#include "interfaces/GUISkiaSysInfo.h"

class bnWindow;
class GPUContextManager {
public:
    static sk_sp<GrDirectContext> getGaneshHeadlessContext();
    static sk_sp<GrDirectContext> getGaneshContextForWindow(const bnWindow* window);
    static void shutdownGaneshContextForWindow(const bnWindow* window);
    bool isGPUAvailable() const { return headlessContext != nullptr; }
private:

#ifdef WIN32
    static sk_sp<GaneshGPUContextType> createD3DContext(ID3D12Device* device,
ID3D12CommandQueue* queue,
IDXGIAdapter1* adapter);
#elif defined(__APPLE__)

#endif

    static sk_sp<GrDirectContext> headlessContext;
    static std::unordered_map<void*, sk_sp<GrDirectContext>> windowContexts;
};
#endif
