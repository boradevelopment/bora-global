#if defined(BORA_UI_SUPPORT) && defined(BORA_HAS_VULKAN)
#include "SkiaVKContextCreator.h"
#include <include/gpu/vk/VulkanBackendContext.h>
#include <include/gpu/vk/VulkanExtensions.h>

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

    auto vkDevice = reinterpret_cast<bnGraphicsVK*>(window->rootDevice);
    skgpu::VulkanBackendContext vkContext;
    vkContext.fDevice = vkDevice->device.Get();
    vkContext.fInstance = vkDevice->instance;
    vkContext.fPhysicalDevice = vkDevice->physicalDevice;
    vkContext.fQueue = vkDevice->graphicsQueue;
    vkContext.fGraphicsQueueIndex = vkDevice->familyIndices.graphicsFamily;
    vkContext.fMaxAPIVersion = vkDevice->GetAPIVersion();
    vkContext.fGetProc = [](const char* name, VkInstance instance, VkDevice device) {
        if (device != VK_NULL_HANDLE) {
            return vkGetDeviceProcAddr(device, name);
        }
        return vkGetInstanceProcAddr(instance, name);
    };
    auto skExtensions = new skgpu::VulkanExtensions();
    skExtensions->init(vkContext.fGetProc, vkDevice->instance, vkDevice->physicalDevice,
                       vkDevice->enabledInstanceExtensions.size(), vkDevice->enabledInstanceExtensions.data(),
                       vkDevice->enabledDeviceExtensions.size(), vkDevice->enabledDeviceExtensions.data());
    vkContext.fVkExtensions = skExtensions;
    extensions.emplace(window->configuration->id, skExtensions);

    return GrDirectContexts::MakeVulkan(vkContext);
}
#endif