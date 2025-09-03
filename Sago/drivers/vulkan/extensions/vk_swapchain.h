#ifndef SG_VK_SWAPCHAIN_H
#define SG_VK_SWAPCHAIN_H
#include <volk.h>
#include <cstdint>
#include <vector>

#include "window/window_sdl.h"
#include "vk_surface.h"
#include "drivers/vulkan/vk_instance.h"
#include "drivers/vulkan/vk_device.h"

namespace Driver::Vulkan {

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentmodes;
};

struct SwapChainProperties{
	uint32_t mini_image_count;
	VkFormat swapchain_image_format;
	VkExtent2D swapchain_extent;
};

class VulkanSwapchain {
public:
	VulkanSwapchain(const Platform::AppWindow&,const VulkanInitializer&, const VulkanSurface&,const VulkanDevice&);
	~VulkanSwapchain() noexcept;

	VulkanSwapchain(const VulkanSwapchain&) = delete;
	VulkanSwapchain& operator=(const VulkanSwapchain&) = delete;
	VulkanSwapchain(VulkanSwapchain&&) = delete;
	VulkanSwapchain& operator=(VulkanSwapchain&&) = delete;

    const std::vector<VkImage>& GetImages() const { return swapchainimages_; }
    const std::vector<VkImageView>& GetImageViews() const { return swapchainimageviews_; }

	const VkExtent2D& GetExtent() const{return swapchainproperties_.swapchain_extent;} 
	const VkFormat& GetFormat() const{return swapchainproperties_.swapchain_image_format;} 

	operator VkSwapchainKHR() const {return swapchain_;}

private:
	SwapChainSupportDetails QuerySwapChainSupport() const;
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const;
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const;
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;
	
	void CreateSwapChain();
	void CreateSwapChainImage();
	void CreateSwapChainImageViews();
private:
	const Platform::AppWindow& window_;
	const VulkanInitializer& init_;
	const VulkanSurface& surface_;
	const VulkanDevice& device_;
private:
	VkSwapchainKHR swapchain_;
	SwapChainProperties swapchainproperties_;
	std::vector<VkImage> swapchainimages_;
	std::vector<VkImageView> swapchainimageviews_;
};

} //namespace Driver::Vulkan

#endif