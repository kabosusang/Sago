#include "vk_surface.h"

#include "drivers/vulkan/extensions/vk_check.h"


namespace Driver::Vulkan {

VulkanSurface::VulkanSurface(const Platform::AppWindow& window,
		const VulkanInitializer& instance) :instance_(instance){

	if (!SDL_Vulkan_CreateSurface(window.GetRawImpl(),
				GetInstance(instance),
				nullptr,
				&surface_)) {
		LogErrorDetaill("[Vulkan][Surface] SDL Vulkan Create Surface");
	}

	// if (!CheckVulkanSupport<CheckType::kSurfaceSupport>(GetPhysicalDevice(instance),surface_)){
	// 	LogErrorDetaill("[Vulkan][Surface] Vulkan Surface No Support");
	// }

	LogInfo("[Vulkan][Init] Create SDL Vulkan Success");


}

VulkanSurface::~VulkanSurface() noexcept {
    vkDestroySurfaceKHR(GetInstance(instance_),surface_,nullptr);

}

VkSurfaceKHR GetSurface(const VulkanSurface& surface){
	return surface.GetSurface();
}


} //namespace Driver::Vulkan