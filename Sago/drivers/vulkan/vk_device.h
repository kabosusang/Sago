#ifndef SG_VK_DEVICE_H
#define SG_VK_DEVICE_H

#include <volk.h>

#include "vk_instance.h"

namespace Driver::Vulkan {

class VulkanDevice {
public:
	VulkanDevice(const VulkanInitializer&);
	~VulkanDevice();

	VulkanDevice(const VulkanDevice&) = delete;
	VulkanDevice& operator=(const VulkanDevice&) = delete;
	VulkanDevice(VulkanDevice&&) = delete;
	VulkanDevice& operator=(VulkanDevice&&) = delete;

private:
	void CreateLogicalDevice();

private:
	const VulkanInitializer& vulkanins_;

private:
	VkDevice device_{};
	VkQueue graphics_queue_{};
};

} //namespace Driver::Vulkan

#endif