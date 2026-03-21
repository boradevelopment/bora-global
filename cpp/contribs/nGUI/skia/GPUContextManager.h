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
    static sk_sp<GaneshGPUContextType> getGaneshHeadlessContext();
    static sk_sp<GaneshGPUContextType> getGaneshContextForWindow(const bnWindow* window);
    static void shutdownGaneshContextForWindow(const bnWindow* window);
    bool isGPUAvailable() const { return headlessContext != nullptr; }
private:
#ifdef WIN32
    static sk_sp<GaneshGPUContextType> createD3DContext(ID3D12Device* device,
ID3D12CommandQueue* queue,
IDXGIAdapter1* adapter);
#endif

    static sk_sp<GaneshGPUContextType> headlessContext;
    static std::unordered_map<void*, sk_sp<GaneshGPUContextType>> windowContexts;
};
#endif
