#include "SkiaVKContextCreator.h"
#include <include/gpu/vk/VulkanBackendContext.h>

#include "nGraphics/bnGraphicsVK.h"
#include "nWindow/bnWindow.h"

sk_sp<GrDirectContext> SkiaVKContextCreator::createContext(VkInstance fInstance,
                                                           VkPhysicalDevice                 fPhysicalDevice,
                                                           VkDevice                         fDevice,
                                                           VkQueue                          fQueue,
                                                           uint32_t                         fGraphicsQueueIndex,
                                                           uint32_t                         fMaxAPIVersion)
 {
     skgpu::VulkanBackendContext vkContext;
     vkContext.fDevice = fDevice;
     vkContext.fInstance = fInstance;
     vkContext.fPhysicalDevice = fPhysicalDevice;
     vkContext.fQueue = fQueue;
     vkContext.fGraphicsQueueIndex = fGraphicsQueueIndex;
     vkContext.fMaxAPIVersion = fMaxAPIVersion;

     return GrDirectContexts::MakeVulkan(vkContext);
 }

sk_sp<GrDirectContext> SkiaVKContextCreator::createContextFromWindow(const bnWindow* window)
{
    if (!window) return nullptr;

    auto vkDevice = dynamic_cast<bnGraphicsVK*>(window->rootDevice);
    skgpu::VulkanBackendContext vkContext;
    vkContext.fDevice = vkDevice->device.Get();
    vkContext.fInstance = vkDevice->instance;
    vkContext.fPhysicalDevice = vkDevice->physicalDevice;
    vkContext.fQueue = vkDevice->graphicsQueue;
    vkContext.fGraphicsQueueIndex = 0; // todo
    vkContext.fMaxAPIVersion = vkDevice->GetAPIVersion();

    return GrDirectContexts::MakeVulkan(vkContext);
}