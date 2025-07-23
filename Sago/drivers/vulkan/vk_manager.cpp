#include "vk_manager.h"

#include "core/io/log/log.h"

namespace driver::vulkan {

void VulkanManager::InitVulkanInstance() {
    if (volkInitialize() != VK_SUCCESS) {
        LogErrorDetaill("Failed to initialize Volk");
        
    }

    VkApplicationInfo appInfo{}; 



}

} //namespace driver::vulkan
