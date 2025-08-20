#ifndef SG_VULKAN_CONTEXT_H
#define SG_VULKAN_CONTEXT_H

//platform
#include "window_interface.h"
//vulkan
#include "drivers/vulkan/vk_instance.h"
#include "drivers/vulkan/vk_device.h"
#include "drivers/vulkan/extensions/vk_surface.h"
#include "drivers/vulkan/extensions/vk_swapchain.h"

namespace Context {

class VulkanContext {
public:
	VulkanContext(const Platform::AppWindow&);
	~VulkanContext();

	auto& GetDevice() { return *vkdevice_; }
	auto& GetSwapChain(){return *vkswapchain_;}

private:
    const Platform::AppWindow& window_;
	std::unique_ptr<Driver::Vulkan::VulkanInitializer> vkinitail_;
	std::unique_ptr<Driver::Vulkan::VulkanDevice> vkdevice_;
	std::unique_ptr<Driver::Vulkan::VulkanSurface> vksurface_;
	std::unique_ptr<Driver::Vulkan::VulkanSwapchain> vkswapchain_;
};

} //namespace Context

#endif