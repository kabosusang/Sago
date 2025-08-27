#ifndef SG_VK_DEVICE_H
#define SG_VK_DEVICE_H

#include "SDL3/SDL_vulkan.h"
#include "extensions/vk_surface.h"
#include "vk_instance.h"


namespace Driver::Vulkan {

class VulkanDevice {
public:
	VulkanDevice(const VulkanInitializer&, const VulkanSurface&);
	~VulkanDevice();

	VulkanDevice(const VulkanDevice&) = delete;
	VulkanDevice& operator=(const VulkanDevice&) = delete;
	VulkanDevice(VulkanDevice&&) = delete;
	VulkanDevice& operator=(VulkanDevice&&) = delete;

	VkDevice GetDevice() const { return device_; }
	VkQueue GetGraphyciQueue() const { return graphics_queue_; }

private:
	void CreateLogicalDevice();
	void CreateDeviceQueue();

private:
	const VulkanInitializer& vulkanins_;

private:
	const VulkanSurface& surface_;
	VkDevice device_{};
	VkQueue graphics_queue_{};
	VkQueue present_queue_{};
};

VkDevice GetDevice(const VulkanDevice&);

} //namespace Driver::Vulkan

#endif