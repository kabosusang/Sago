#include "vk_surface.h"

#include "drivers/vulkan/extensions/vk_check.h"
#include <type_traits>

namespace Driver::Vulkan {

VulkanSurface::VulkanSurface(const Platform::AppWindow& window,
		const VulkanInitializer& instance) :
		instance_(instance) {
	if (!SDL_Vulkan_CreateSurface(window.GetRawImpl(),
				instance,
				nullptr,
				&surface_)) {
		LogErrorDetail("[Vulkan][Surface] SDL Vulkan Create Surface");
	}
	const VkPhysicalDevice& pdevice = GetPhysicalDevice(instance);
	const VkSurfaceKHR& surface = surface_;

	if (!CheckVulkanSupport<CheckType::kSurfaceSupport>(pdevice, surface)) {
		LogErrorDetail("[Vulkan][Surface] Vulkan Surface No Support");
	}

	LogInfo("[Vulkan][Init] Create SDL Vulkan Success");
}

VulkanSurface::~VulkanSurface() noexcept {
	vkDestroySurfaceKHR(instance_, surface_, nullptr);
}

VkSurfaceKHR GetSurface(const VulkanSurface& surface) {
	return surface.GetSurface();
}

} //namespace Driver::Vulkan