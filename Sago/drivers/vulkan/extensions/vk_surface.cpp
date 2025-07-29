#include "vk_surface.h"

namespace Driver::Vulkan {

VulkanSurface::VulkanSurface(const Platform::AppWindow& window,
		const VulkanInitializer& instance) :instance_(instance){
	if (!SDL_Vulkan_CreateSurface(window.GetRawImpl(),
				GetInstance(instance),
				nullptr,
				&surface_)) {
		LogErrorDetaill("[Vulkan][Surface] SDL Vulkan Create Surface");
	}

	LogInfo("[Vulkan][Init] Create SDL Vulkan Success");

	auto indice = FindIndice(Presente{},GetPhysicalDevice(instance),surface_);
	if (!indice.isComplete()){
		LogErrorDetaill("[Vulkan][Surface] Physical Device No SurfaceSupport");
	}

}

VulkanSurface::~VulkanSurface() noexcept {
    vkDestroySurfaceKHR(GetInstance(instance_),surface_,nullptr);

}




} //namespace Driver::Vulkan