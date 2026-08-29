// Apart of the BORA Runtime Source which uses the TAOSU License
// Check LICENSE.md for more information regarding the BORA license.
#ifdef BORA_UI_SUPPORT
#include "GPUContextManager.h"

#include "common/SkiaVKContextCreator.h"
#include "common/SkiaVKRenderer.h"
#include "nWindow/bnWindow.h"
#include "darwin/SkiaMTLContextCreator.h"

inline std::unordered_map<void*, sk_sp<GrDirectContext>> GPUContextManager::windowContexts;

sk_sp<GrDirectContext> GPUContextManager::getGaneshHeadlessContext() {
    return nullptr;
}

sk_sp<GrDirectContext> GPUContextManager::getGaneshContextForWindow(const bnWindow *window) {
    if (!window) return nullptr;
#ifdef WIN32
    if (window->choice == D3D12) {
        if (windowContexts[window->handle]) {
            return windowContexts[window->handle];
        } else {
            auto graphObject = reinterpret_cast<bnGraphicsD3D12*>(window->gEngineExpl);
            if (!graphObject) return nullptr;
            auto deviceObj = (DeviceD3D12*)graphObject->getDevice();
            windowContexts[window->handle] = createD3DContext((ID3D12Device*)deviceObj->GetNativeHandle(), graphObject->GetCommandQueue(), graphObject->GetAdapter());
            return windowContexts[window->handle];
        }
    }
#elif defined(__APPLE__)
    if (window->choice == METAL) {
        if (windowContexts[window->handle]) {
            return windowContexts[window->handle];
        } else {
            windowContexts[window->handle] = SkiaMTLContextCreator::createNewContextFromWindow(window);
        }
    }
#endif

    if (window->choice == VULKAN)
    {
        if (windowContexts[window->handle]) {
            return windowContexts[window->handle];
        } else {
            windowContexts[window->handle] = SkiaVKContextCreator::createContextFromWindow(window);
            return windowContexts[window->handle];
        }
    }


    return nullptr;
}

// Use responsibly, as this just resets the object. This is called at the end before graphics engine is killed.
void GPUContextManager::shutdownGaneshContextForWindow(const bnWindow *window) {
    if (windowContexts[window->handle]) {
        windowContexts[window->handle].reset();
        windowContexts.erase(window->handle);
    }
}


#ifdef WIN32
sk_sp<GrDirectContext> GPUContextManager::createD3DContext(ID3D12Device *device, ID3D12CommandQueue *queue,
                                                           IDXGIAdapter1 *adapter) {
    if (!device || !queue || !adapter) return nullptr;

    GrD3DBackendContext backendCtx = {};
    backendCtx.fDevice = gr_cp(device);
    backendCtx.fQueue = gr_cp(queue);
    backendCtx.fProtectedContext = GrProtected::kNo;
    backendCtx.fMemoryAllocator = nullptr;
    backendCtx.fAdapter = gr_cp(adapter);


    sk_sp<GrDirectContext> grContext = GrDirectContext::MakeDirect3D(backendCtx);

    if (!grContext) {
        // Failed to create GPU context, maybe driver too old
        return nullptr;
    }

    return grContext;
}
#endif
#endif