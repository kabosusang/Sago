#include "vulkan_context.h"

namespace Context {

VulkanContext::VulkanContext(const Platform::AppWindow& window) :
		window_(window) {
	using namespace Driver::Vulkan;
	vkinitail_ = std::make_unique<VulkanInitializer>();
	vksurface_ = std::make_unique<VulkanSurface>(window_, *vkinitail_);
	vkdevice_ = std::make_unique<VulkanDevice>(*vkinitail_, *vksurface_);
	vkswapchain_ = std::make_unique<VulkanSwapchain>(window_, *vkinitail_, *vksurface_, *vkdevice_);
}

VulkanContext::~VulkanContext() {
    
}

} //namespace Context