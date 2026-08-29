#pragma once
#if defined(BORA_UI_SUPPORT) && defined(BORA_HAS_VULKAN)
#include <vulkan/vulkan.h>
#include "nGUI/skia/interfaces/GUISkiaSysInfo.h"

class bnWindow;

class SkiaVKContextCreator {
public:
    static sk_sp<GrDirectContext> createContext(VkInstance fInstance,
    VkPhysicalDevice                 fPhysicalDevice,
    VkDevice                         fDevice,
    VkQueue                          fQueue,
    uint32_t                         fGraphicsQueueIndex = 0,
    uint32_t                         fMaxAPIVersion = 0);

    static sk_sp<GrDirectContext> createContextFromWindow(const bnWindow* window);
};
#endif