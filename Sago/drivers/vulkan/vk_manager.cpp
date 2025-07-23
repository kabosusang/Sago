#include "vk_manager.h"


#define VOLK_IMPLEMENTATION
#include "volk.h"
#include "core/io/log/log.h"

namespace Driver::Vulkan {

void VkContextRendering::Init(){
    InitVulkanInstance();
}


void VkContextRendering::InitVulkanInstance() {
    if (volkInitialize() != VK_SUCCESS) {
        LogErrorDetaill("Failed to initialize Volk");
    }
    VkApplicationInfo appInfo{}; 
}





} //namespace driver::vulkan
