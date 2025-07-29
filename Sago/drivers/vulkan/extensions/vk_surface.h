#ifndef SG_VK_SURFACE_H
#define SG_VK_SURFACE_H

#include <SDL3/SDL_vulkan.h>

#include "window/window_sdl.h"
#include "drivers/vulkan/vk_instance.h"

namespace Driver::Vulkan {

class VulkanSurface {
public:
	VulkanSurface(const Platform::AppWindow&,const VulkanInitializer&);
	~VulkanSurface() noexcept;

	VulkanSurface(const VulkanSurface&) = delete;
	VulkanSurface& operator=(const VulkanSurface&) = delete;
	VulkanSurface(VulkanSurface&&) = delete;
	VulkanSurface& operator=(VulkanSurface&&) = delete;

private:
    const VulkanInitializer& instance_;
	VkSurfaceKHR surface_{};
    VkQueue present_queue_{};
};

} //namespace Driver::Vulkan

#endif