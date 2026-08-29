#if defined(BORA_UI_SUPPORT) && defined(__APPLE__)
#include "SkiaMTLContextCreator.h"
#include "nGraphics/bnGraphicsMTL.h"

sk_sp<GaneshGPUContextType> SkiaMTLContextCreator::createMTLContext(void* device, void* queue) {
    if(!device || !queue) return nullptr;

    GrMtlBackendContext context = {};
    context.fDevice = sk_ret_cfp((const void*)device);
    context.fQueue = sk_ret_cfp((const void*)queue);
    sk_sp<GrDirectContext> grContext = GrDirectContexts::MakeMetal(context);

    if (!grContext) {
        return nullptr;
    }

    return grContext;
}

sk_sp<GaneshGPUContextType> SkiaMTLContextCreator::createNewContextFromWindow(const bnWindow *window) {
    if(!window) return nullptr;
    auto graphObject = reinterpret_cast<bnGraphicsMTL*>(window->gEngineExpl);
    if (!graphObject) return nullptr;
    auto deviceObj = (DeviceMTL*)graphObject->getDevice();
    auto context = createMTLContext((id<MTLDevice>)deviceObj->GetNativeHandle(), (id<MTLCommandQueue>)graphObject->getCommandQueue());
    return context;
}
#endif