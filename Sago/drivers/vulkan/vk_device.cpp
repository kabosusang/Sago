#include "vk_device.h"

#include "core/io/log/log.h"



namespace Driver::Vulkan{

VulkanDevice::~VulkanDevice(){
    LogInfo("Vulkan Device Release");
}

VulkanDevice::VulkanDevice(){
    CreateLogicalDevice();
    LogInfo("Vulkan Device Created");
}

void VulkanDevice::CreateLogicalDevice() const{



}





    
}



