#ifndef SG_VK_SURFACE_H
#define SG_VK_SURFACE_H

#include <SDL3/SDL_vulkan.h>

#include "drivers/vulkan/vk_instance.h"
#include "window/window_sdl.h"

namespace Driver::Vulkan {

class VulkanSurface {
public:
	VulkanSurface(const Platform::AppWindow&, const VulkanInitializer&);
	~VulkanSurface() noexcept;

	VulkanSurface(const VulkanSurface&) = delete;
	VulkanSurface& operator=(const VulkanSurface&) = delete;
	VulkanSurface(VulkanSurface&&) = delete;
	VulkanSurface& operator=(VulkanSurface&&) = delete;

	inline VkSurfaceKHR GetSurface() const { return surface_; }
	operator VkSurfaceKHR() const { return surface_; }

private:
	const VulkanInitializer& instance_;
	VkSurfaceKHR surface_{};
};

VkSurfaceKHR GetSurface(const VulkanSurface&);

} //namespace Driver::Vulkan

#endif