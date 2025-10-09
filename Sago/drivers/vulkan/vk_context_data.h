#ifndef SG_VK_CONTEXT_DATA_H
#define SG_VK_CONTEXT_DATA_H
#include <volk.h>

namespace Driver::Vulkan {
    struct VulkanContextData{
        VkDevice device = VK_NULL_HANDLE;
        VkInstance instance = VK_NULL_HANDLE;
        VkPhysicalDevice physical_device = VK_NULL_HANDLE;
        VkQueue queue = VK_NULL_HANDLE;
        VkFormat format = VK_FORMAT_UNDEFINED;
    };


}







#endif