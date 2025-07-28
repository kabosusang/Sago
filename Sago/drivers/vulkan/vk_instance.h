#ifndef SG_VK_INSTANCE_H
#define SG_VK_INSTANCE_H
#include <cstdint>
#include <optional>
#include <vector>

#include "SDL3/SDL_vulkan.h"
#include "util/vk_queue_faimly.h"
#include "window_interface.h"


namespace Driver::Vulkan {
class SDL_Window;

class VulkanInitializer {
public:
	VulkanInitializer();
	~VulkanInitializer() noexcept;

	VulkanInitializer(const VulkanInitializer&) = delete;
	VulkanInitializer& operator=(const VulkanInitializer&) = delete;
	VulkanInitializer(VulkanInitializer&&) = delete;
	VulkanInitializer& operator=(VulkanInitializer&&) = delete;
	void Init();

	VkInstance GetInstance() const{ return instance_; }
	VkPhysicalDevice GetPhysicalDevice() const {return physical_device_;};
private:
	void CheckRequireDextensionSupport(std::vector<const char*>&) const;
	bool CheckValidationLayerSupport(const std::vector<const char*>&) const;
	bool CheckIsDeviceSuitable(const VkPhysicalDevice&) const;

	void InitVulkanInstance();
	void PickPhysicalDevice();

private:
	//Instance
	VkInstance instance_{};
	VkPhysicalDevice physical_device_{};
};

} //namespace Driver::Vulkan

#endif