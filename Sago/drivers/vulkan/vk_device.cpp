#include "vk_device.h"

#include "core/io/log/log.h"
#include "vk_log.h"
#include "util/vk_queue_faimly.h"

namespace Driver::Vulkan {

VulkanDevice::~VulkanDevice() {
	LogInfo("[Vulkan][Destory] Device Release");
    vkDestroyDevice(device_, nullptr);
}

VulkanDevice::VulkanDevice(const VulkanInitializer& vulkanins) :
		vulkanins_(vulkanins) {
	CreateLogicalDevice();
	LogInfo("[Vulkan][Init] Device Created");
}

void VulkanDevice::CreateLogicalDevice(){
	auto&& indices = FindIndice(Graphy{},GetPhysicalDevice(vulkanins_));

	VkDeviceQueueCreateInfo queue_create_info{};
	queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_create_info.queueFamilyIndex = indices.family_.value();
	queue_create_info.queueCount = 1;

	const float queue_priority = 1.0f;
	queue_create_info.pQueuePriorities = &queue_priority;

	VkPhysicalDeviceFeatures device_features{};

	VkDeviceCreateInfo create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	create_info.pQueueCreateInfos = &queue_create_info;
	create_info.queueCreateInfoCount = 1;
	create_info.pEnabledFeatures = &device_features;

    auto result = vkCreateDevice(GetPhysicalDevice(vulkanins_), &create_info, nullptr, &device_);
    if (result != VK_SUCCESS){
        VK_LOG_ERROR("[Vulkan][Init] Create Device: ",result);
    }
    vkGetDeviceQueue(device_, indices.family_.value(), 0, &graphics_queue_);
    //Only One Device
    volkLoadDevice(device_);
}












} //namespace Driver::Vulkan
