#ifndef SG_VK_INSTANCE_H
#define SG_VK_INSTANCE_H
#include <cstdint>
#include <optional>
#include <vector>


#include "SDL3/SDL_vulkan.h"
#include "window_interface.h"

namespace Driver::Vulkan {
class SDL_Window;

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicis_family_;
	inline bool isComplete() const {
		return graphicis_family_.has_value();
	}
};

inline QueueFamilyIndices FindQueueFamilise(const VkPhysicalDevice&);


class VulkanInitializer {
public:
	VulkanInitializer();
	~VulkanInitializer() noexcept;

	VulkanInitializer(const VulkanInitializer&) = delete;
	VulkanInitializer& operator=(const VulkanInitializer&) = delete;
	VulkanInitializer(VulkanInitializer&&) = delete;
	VulkanInitializer& operator=(VulkanInitializer&&) = delete;
	void Init();
private:
	void CheckRequireDextensionSupport(std::vector<const char*>&) const;
	bool CheckValidationLayerSupport(const std::vector<const char*>&) const;
	bool CheckIsDeviceSuitable(const VkPhysicalDevice&) const;

	void InitVulkanInstance();
	void PickPhysicalDevice();

private:
	SDL_Window* window_;

	//Instance
	std::vector<const char*> instanceExtensionNames_;
	VkInstance instance_{};
	VkPhysicalDevice physical_device_{};
	//VkDevice device_{};
};

} //namespace Driver::Vulkan

#endif