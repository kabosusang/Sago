#ifndef SG_VULKAN_MEMORY_REQUIREMENT_H
#define SG_VULKAN_MEMORY_REQUIREMENT_H
#include <volk.h>
#include <cstdint>


namespace Driver::Vulkan::Memory {

class Requirement {
public:
    static VkMemoryRequirements MemoryRequirements(const VkDevice& device,const VkBuffer& buffer){
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, buffer, &memRequirements);
        return memRequirements;
    }

    static uint32_t FindMemoryType(const VkPhysicalDevice& physicalDevice,uint32_t typefilter,VkMemoryPropertyFlags properties){
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typefilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }
    }

};

} //namespace Driver::Vulkan::Memory

#endif
