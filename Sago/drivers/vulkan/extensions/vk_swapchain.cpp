#include "vk_swapchain.h"

#include "core/io/log/log.h"
#include "vk_surface.h"
#include <algorithm>

namespace Driver::Vulkan {

SwapChainSupportDetails VulkanSwapchain::QuerySwapChainSupport() const {
	SwapChainSupportDetails details;

	auto device = GetPhysicalDevice(init_);
	auto surface = GetSurface(surface_);
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

VkSurfaceFormatKHR VulkanSwapchain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const {
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}
	return availableFormats[0];
}
VkPresentModeKHR VulkanSwapchain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const {
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanSwapchain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const {
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	} else {
		int width, height;
		SDL_GetWindowSizeInPixels(Platform::GetWindowPtr(window_), &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

VulkanSwapchain::VulkanSwapchain(const Platform::AppWindow& window, const VulkanInitializer& init, const VulkanSurface& surface,const VulkanDevice& device) :
		window_(window), init_(init), surface_(surface),device_(device) {
	CreateSwapChain();
	LogInfo("[Vulkan][Init] Create SwapChain Success");
}

VulkanSwapchain::~VulkanSwapchain() noexcept{
    vkDestroySwapchainKHR(GetDevice(device_), swapchain_, nullptr);
    LogInfo("[Vulkan][Destory] Destory SwapChain");
}


void VulkanSwapchain::CreateSwapChain() {
	SwapChainSupportDetails swapchain_support = QuerySwapChainSupport();
	auto swap_chain_adequate = !swapchain_support.formats.empty() && !swapchain_support.presentModes.empty();
	if (!swap_chain_adequate) {
		LogErrorDetaill("[Vulkan][SwapChain] SwapChain No Support Adequate");
	}

	VkSurfaceFormatKHR surface_format = ChooseSwapSurfaceFormat(swapchain_support.formats);
	VkPresentModeKHR present_mode = ChooseSwapPresentMode(swapchain_support.presentModes);
	VkExtent2D extent = ChooseSwapExtent(swapchain_support.capabilities);

	uint32_t image_count = swapchain_support.capabilities.minImageCount + 1;

	if (swapchain_support.capabilities.maxImageCount > 0 && image_count > swapchain_support.capabilities.maxImageCount) {
		image_count = swapchain_support.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.surface = GetSurface(surface_);

	create_info.minImageCount = image_count;
	create_info.imageFormat = surface_format.format;
	create_info.imageColorSpace = surface_format.colorSpace;
	create_info.imageExtent = extent;
	create_info.imageArrayLayers = 1;
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	auto indice_graphy = FindIndice(Graphy{}, GetPhysicalDevice(init_));
	auto indice_present = FindIndice(Presente{}, GetPhysicalDevice(init_), GetSurface(surface_));

	uint32_t queue_family_indices[] = { indice_graphy.family_.value(), indice_present.family_.value() };

	if (indice_graphy.family_ != indice_present.family_) {
		create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		create_info.queueFamilyIndexCount = 2;
		create_info.pQueueFamilyIndices = queue_family_indices;
	} else {
		create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		create_info.queueFamilyIndexCount = 0; // Optional
		create_info.pQueueFamilyIndices = nullptr; // Optional
	}

	create_info.preTransform = swapchain_support.capabilities.currentTransform;
	create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	create_info.presentMode = present_mode;
	create_info.clipped = VK_TRUE;
	create_info.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(GetDevice(device_), &create_info, nullptr, &swapchain_) != VK_SUCCESS) {
		LogErrorDetaill("[Vulkan][SwapChain] Failed to Create SwapChain");
	}
}

} //namespace Driver::Vulkan
