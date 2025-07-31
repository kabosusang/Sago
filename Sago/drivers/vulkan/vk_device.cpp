#include "vk_device.h"

#include <array>
#include <cstdint>
#include <set>

#include "core/io/log/log.h"
#include "extensions/vk_surface.h"
#include "util/vk_queue_faimly.h"
#include "extensions/vk_check.h"
#include "vk_instance.h"
#include "vk_log.h"


namespace Driver::Vulkan {

VulkanDevice::~VulkanDevice() {
	vkDestroyDevice(device_, nullptr);
	LogInfo("[Vulkan][Destory] Destory Device");
}

VulkanDevice::VulkanDevice(const VulkanInitializer& vulkanins, const VulkanSurface& surface) :
		vulkanins_(vulkanins), surface_(surface) {
	CreateLogicalDevice();
	LogInfo("[Vulkan][Init] Device Created Success");
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

	//Check Physical Device Extensions
	const std::vector<const char*> device_extensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	const auto& pdevice = GetPhysicalDevice(vulkanins_);
	if (!CheckVulkanSupport<CheckType::kDeviceExtensions>(device_extensions,pdevice)) {
		LogErrorDetaill("[Vulkan][PhysicalDevice] Failed to Device Extensions");
	}
	
	VkPhysicalDeviceFeatures device_features{};

	VkDeviceCreateInfo create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	create_info.pQueueCreateInfos = queue_create_infos.data();
	create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
	create_info.pEnabledFeatures = &device_features;
	create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
	create_info.ppEnabledExtensionNames = device_extensions.data();


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

VkDevice GetDevice(const VulkanDevice& device){
	return device.GetDevice();
}


} //namespace Driver::Vulkan
