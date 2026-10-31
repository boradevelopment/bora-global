#if defined(BORA_UI_SUPPORT) && defined(BORA_HAS_OPENGL)
#include "SkiaOGLContextCreator.h"
#include <include/gpu/ganesh/gl/GrGLDirectContext.h>
#include <include/gpu/ganesh/gl/GrGLInterface.h>
#include "nGraphics/bnGraphicsOGL.h"
#include "nWindow/bnWindow.h"
#include <include/gpu/ganesh/gl/GrGLAssembleInterface.h>

sk_sp<GrDirectContext> SkiaOGLontextCreator::createContextFromWindow(const bnWindow* window)
{
    if (!window) return nullptr;

    auto oglDevice = dynamic_cast<bnGraphicsOGL*>(window->rootDevice);
    sk_sp<const GrGLInterface> glInterface = GrGLMakeAssembledInterface(
        nullptr,
        [](void* ctx, const char* name) -> GrGLFuncPtr {
            return eglGetProcAddress(name);
        }
    );

    if (!glInterface) {
        glInterface = GrGLMakeNativeInterface();
    }

    if (!glInterface) {
        return nullptr; // Driver or context interface binding failed
    }

    GrContextOptions oglOptions;
    return GrDirectContexts::MakeGL(glInterface, oglOptions);
}
#endif