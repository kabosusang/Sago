#ifndef SG_VULKAN_COMMAND_POOL_H
#define SG_VULKAN_COMMAND_POOL_H
#include <volk.h>

#include "vk_command.h"

namespace Driver::Vulkan {

class VulkanCommandPool {
public:
	VulkanCommandPool(VkDevice device, uint32_t queueFamilyIndex);
	~VulkanCommandPool();

	VulkanCommandPool(const VulkanCommandPool&) = delete;
	VulkanCommandPool& operator=(const VulkanCommandPool&) = delete;
	VulkanCommandPool(VulkanCommandPool&&) = delete;
	VulkanCommandPool& operator=(VulkanCommandPool&&) = delete;

	inline std::unique_ptr<VulkanCommand> CreateCommand(VkQueue queue) {
		return std::make_unique<VulkanCommand>(device_, commandpool_, queue);
	}

private:
	VkCommandPool commandpool_;
	VkDevice device_;
};

} //namespace Driver::Vulkan

#endif