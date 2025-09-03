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
	VkInstance GetInstance() const{ return instance_; }
	VkPhysicalDevice GetPhysicalDevice() const {return physical_device_;};

	operator VkInstance() const {return instance_;}
	operator VkPhysicalDevice() const {return physical_device_;}
private:

	void Init();
	void InitVulkanInstance();
	void PickPhysicalDevice();
private:
	//Instance
	VkInstance instance_{};
	VkPhysicalDevice physical_device_{};
};

VkInstance GetInstance(const VulkanInitializer&);
VkPhysicalDevice GetPhysicalDevice(const VulkanInitializer&);


} //namespace Driver::Vulkan

#endif