#include "vk_device.h"

#include "core/io/log/log.h"
#include "extensions/vk_surface.h"
#include "util/vk_queue_faimly.h"
#include "vk_log.h"
#include <array>
#include <cstdint>
#include <set>

namespace Driver::Vulkan {

VulkanDevice::~VulkanDevice() {
	LogInfo("[Vulkan][Destory] Device Release");
	vkDestroyDevice(device_, nullptr);
}

VulkanDevice::VulkanDevice(const VulkanInitializer& vulkanins, const VulkanSurface& surface) :
		vulkanins_(vulkanins), surface_(surface) {
	CreateLogicalDevice();
	LogInfo("[Vulkan][Init] Device Created");
}

void VulkanDevice::CreateLogicalDevice() {
	auto indice_graphy = FindIndice(Graphy{}, GetPhysicalDevice(vulkanins_));
	auto indice_present = FindIndice(Presente{}, GetPhysicalDevice(vulkanins_), GetSurface(surface_));

	std::set<uint32_t> unique_queue_familes = {
		indice_graphy.family_.value(),
		indice_present.family_.value()
	};

	std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
	
	const float queuePriority = 1.0f;
	for (auto queueFamily : unique_queue_familes) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queue_create_infos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures device_features{};

	VkDeviceCreateInfo create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	create_info.pQueueCreateInfos = queue_create_infos.data();
	create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
	create_info.pEnabledFeatures = &device_features;
	create_info.enabledExtensionCount = 0;

	auto result = vkCreateDevice(GetPhysicalDevice(vulkanins_), &create_info, nullptr, &device_);
	if (result != VK_SUCCESS) {
		VK_LOG_ERROR("[Vulkan][Init] Create Device: ", result);
	}
	vkGetDeviceQueue(device_, indice_graphy.family_.value(), 0, &graphics_queue_);
	vkGetDeviceQueue(device_, indice_present.family_.value(), 0, &present_queue_);

	//Only One Device
	volkLoadDevice(device_);
}

void VulkanDevice::CreateDeviceQueue() {
	auto indice_graphy = FindIndice(Graphy{}, GetPhysicalDevice(vulkanins_));
	auto indice_present = FindIndice(Presente{}, GetPhysicalDevice(vulkanins_));

	std::array<uint32_t, 2> unique_queue_familes = { indice_graphy.family_.value(), indice_present.family_.value() };
	std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
	float queuePriority = 1.0f;
	for (auto queueFamily : unique_queue_familes) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queue_create_infos.push_back(queueCreateInfo);
	}
}

} //namespace Driver::Vulkan
