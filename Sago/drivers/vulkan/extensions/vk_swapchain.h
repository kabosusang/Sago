#ifndef SG_VK_SWAPCHAIN_H
#define SG_VK_SWAPCHAIN_H

namespace Driver::Vulkan {

class VulkanSwapchain {
public:
	VulkanSwapchain();
	~VulkanSwapchain() noexcept;

	VulkanSwapchain(const VulkanSwapchain&) = delete;
	VulkanSwapchain& operator=(const VulkanSwapchain&) = delete;
	VulkanSwapchain(VulkanSwapchain&&) = delete;
	VulkanSwapchain& operator=(VulkanSwapchain&&) = delete;

private:


};

} //namespace Driver::Vulkan

#endif