#include "vk_swapchain.h"

#include "drivers/vulkan/vk_log.h"
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
		details.presentmodes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentmodes.data());
	}

	return details;
}

VkSurfaceFormatKHR VulkanSwapchain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const {
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
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
		auto [width, height] = window_.GetWindowSizeInPixel();

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

VulkanSwapchain::VulkanSwapchain(const Platform::AppWindow& window, const VulkanInitializer& init, const VulkanSurface& surface, const VulkanDevice& device) :
		window_(window), init_(init), surface_(surface), device_(device) {
	CreateSwapChain();
	LogInfo("[Vulkan][Init] Create SwapChain Success");
	CreateSwapChainImage();
	LogInfo("[Vulkan][Init] Create SwapChainImage Success");
	CreateSwapChainImageViews();
	LogInfo("[Vulkan][Init] Create SwapChainImage Success");
}

VulkanSwapchain::~VulkanSwapchain() noexcept {
	const auto& device = GetDevice(device_);

	vkDestroySwapchainKHR(device, swapchain_, nullptr);
	LogInfo("[Vulkan][Destory] Destory SwapChain");
	for (auto& imageView : swapchainimageviews_) {
		if (imageView) {
			vkDestroyImageView(device, imageView, nullptr);
		}
	}
	LogInfo("[Vulkan][Destory] Destory SwapChain ImageView");
}

bool VulkanSwapchain::CreateSwapChain() {
	SwapChainSupportDetails swapchain_support = QuerySwapChainSupport();
	auto swap_chain_adequate = !swapchain_support.formats.empty() && !swapchain_support.presentmodes.empty();
	if (!swap_chain_adequate) [[unlikely]] {
		LogErrorDetail("[Vulkan][SwapChain] SwapChain No Support Adequate");
	}

	VkSurfaceFormatKHR surface_format = ChooseSwapSurfaceFormat(swapchain_support.formats);
	VkPresentModeKHR present_mode = ChooseSwapPresentMode(swapchain_support.presentmodes);
	VkExtent2D extent = ChooseSwapExtent(swapchain_support.capabilities);
	if (extent.height == 0 || extent.width == 0) {
		return false;
	}

	uint32_t image_count = swapchain_support.capabilities.minImageCount + 1;

	if (swapchain_support.capabilities.maxImageCount > 0 && image_count > swapchain_support.capabilities.maxImageCount) {
		image_count = swapchain_support.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.surface = surface_;

	create_info.minImageCount = image_count;
	create_info.imageFormat = surface_format.format;
	create_info.imageColorSpace = surface_format.colorSpace;
	create_info.imageExtent = extent;
	create_info.imageArrayLayers = 1;
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	auto indice_graphy = FindIndice(Graphy{}, init_);
	auto indice_present = FindIndice(Presente{}, init_, surface_);

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

	if (vkCreateSwapchainKHR(device_, &create_info, nullptr, &swapchain_) != VK_SUCCESS) [[unlikely]] {
		LogErrorDetail("[Vulkan][SwapChain] Failed to Create SwapChain");
	}

	swapchainproperties_.mini_image_count = image_count;
	swapchainproperties_.swapchain_image_format = surface_format.format;
	swapchainproperties_.swapchain_extent = extent;
	return true;
}

void VulkanSwapchain::CreateSwapChainImage() {
	vkGetSwapchainImagesKHR(device_, swapchain_, &swapchainproperties_.mini_image_count, nullptr);
	swapchainimages_.resize(swapchainproperties_.mini_image_count);
	vkGetSwapchainImagesKHR(device_, swapchain_, &swapchainproperties_.mini_image_count, swapchainimages_.data());
}

void VulkanSwapchain::CreateSwapChainImageViews() {
	swapchainimageviews_.resize(swapchainimages_.size());

	for (size_t i = 0; i < swapchainimages_.size(); i++) {
		VkImageViewCreateInfo create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		create_info.image = swapchainimages_[i];
		create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;

		create_info.format = swapchainproperties_.swapchain_image_format;
		create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		create_info.subresourceRange.baseMipLevel = 0;
		create_info.subresourceRange.levelCount = 1;
		create_info.subresourceRange.baseArrayLayer = 0;
		create_info.subresourceRange.layerCount = 1;

		create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		create_info.subresourceRange.baseMipLevel = 0;
		create_info.subresourceRange.levelCount = 1;
		create_info.subresourceRange.baseArrayLayer = 0;
		create_info.subresourceRange.layerCount = 1;

		if (auto result = vkCreateImageView(device_, &create_info, nullptr, &swapchainimageviews_[i]); result != VK_SUCCESS) {
			VK_LOG_ERROR("Failed to create SwapChain ImageView: ", result);
		}
	}
}

void VulkanSwapchain::RecreateSwapchain() {
	CleanSwapChain();

	swapchain_ = VK_NULL_HANDLE;
	swapchainimages_.clear();
	swapchainimageviews_.clear();

	if (!CreateSwapChain()) {
		return;
	}

	CreateSwapChainImage();
	CreateSwapChainImageViews();
}

void VulkanSwapchain::CleanSwapChain() {
	const auto& device = GetDevice(device_);

	for (auto& imageView : swapchainimageviews_) {
		if (imageView != VK_NULL_HANDLE) {
			vkDestroyImageView(device, imageView, nullptr);
			imageView = VK_NULL_HANDLE;
		}
	}
	swapchainimageviews_.clear();

	if (swapchain_ != VK_NULL_HANDLE) {
		vkDestroySwapchainKHR(device, swapchain_, nullptr);
		swapchain_ = VK_NULL_HANDLE;
	}
	swapchainimages_.clear();
}

} //namespace Driver::Vulkan
