#ifndef SG_VK_SWAPCHAIN_H
#define SG_VK_SWAPCHAIN_H
#include <volk.h>
#include <vector>

#include "window/window_sdl.h"
#include "vk_surface.h"
#include "drivers/vulkan/vk_instance.h"
#include "drivers/vulkan/vk_device.h"

namespace Driver::Vulkan {

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class VulkanSwapchain {
public:
	VulkanSwapchain(const Platform::AppWindow&,const VulkanInitializer&, const VulkanSurface&,const VulkanDevice&);
	~VulkanSwapchain() noexcept;

	VulkanSwapchain(const VulkanSwapchain&) = delete;
	VulkanSwapchain& operator=(const VulkanSwapchain&) = delete;
	VulkanSwapchain(VulkanSwapchain&&) = delete;
	VulkanSwapchain& operator=(VulkanSwapchain&&) = delete;

private:
	SwapChainSupportDetails QuerySwapChainSupport() const;
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const;
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const;
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;
	void CreateSwapChain();

private:
	const Platform::AppWindow& window_;
	const VulkanInitializer& init_;
	const VulkanSurface& surface_;
	const VulkanDevice& device_;
	VkSwapchainKHR swapchain_;
};

} //namespace Driver::Vulkan

#endif