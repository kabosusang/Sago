#include "vk_device.h"

#include <array>
#include <cstdint>
#include <set>

#include "core/io/log/log.h"
#include "extensions/vk_check.h"
#include "extensions/vk_surface.h"
#include "util/vk_queue_faimly.h"
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
	auto indice_graphy = FindIndice(Graphy{}, vulkanins_);
	auto indice_present = FindIndice(Presente{}, vulkanins_, surface_);

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
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_EXT_MEMORY_BUDGET_EXTENSION_NAME,
		VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME, //dynamic rendererpass
		VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME
	};

	VkPhysicalDeviceDynamicRenderingFeatures dynamic_rendering_feature = {};
	dynamic_rendering_feature.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
	dynamic_rendering_feature.dynamicRendering = VK_TRUE;

	VkPhysicalDeviceSynchronization2Features synchronization2_feature = {};
	synchronization2_feature.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
	synchronization2_feature.synchronization2 = VK_TRUE;
	synchronization2_feature.pNext = &dynamic_rendering_feature;

	VkPhysicalDeviceVulkan13Features vulkan13_features = {};
	vulkan13_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
	vulkan13_features.dynamicRendering = VK_TRUE;
	vulkan13_features.synchronization2 = VK_TRUE;
	vulkan13_features.pNext = &dynamic_rendering_feature;

	VkPhysicalDeviceFeatures2 device_features2 = {};
	device_features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	device_features2.pNext = &vulkan13_features;

	const auto& pdevice = GetPhysicalDevice(vulkanins_);
	if (!CheckVulkanSupport<CheckType::kDeviceExtensions>(device_extensions, pdevice)) {
		LogErrorDetail("[Vulkan][PhysicalDevice] Failed to Device Extensions");
	}

	VkPhysicalDeviceFeatures device_features{};

	VkDeviceCreateInfo create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	create_info.pQueueCreateInfos = queue_create_infos.data();
	create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
	create_info.pEnabledFeatures = &device_features;
	create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
	create_info.ppEnabledExtensionNames = device_extensions.data();
	//create_info.pNext = &device_features2;

	if (auto result = vkCreateDevice(vulkanins_, &create_info, nullptr, &device_); result != VK_SUCCESS) {
		VK_LOG_ERROR("[Vulkan][Init] Create Device: ", result);
	}
	vkGetDeviceQueue(device_, indice_graphy.family_.value(), 0, &graphics_queue_);
	vkGetDeviceQueue(device_, indice_present.family_.value(), 0, &present_queue_);

	//Only One Device
	volkLoadDevice(device_);
}

void VulkanDevice::CreateDeviceQueue() {
	auto indice_graphy = FindIndice(Graphy{},vulkanins_);
	auto indice_present = FindIndice(Presente{}, vulkanins_);

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

VkDevice GetDevice(const VulkanDevice& device) {
	return device.GetDevice();
}

} //namespace Driver::Vulkan
