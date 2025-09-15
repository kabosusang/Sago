#include "vk_commandpool.h"

namespace Driver::Vulkan {

VulkanCommandPool::VulkanCommandPool(VkDevice device, uint32_t queueFamilyIndex) :device_(device){
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndex;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // 允许重置命令缓冲区

	if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandpool_) != VK_SUCCESS) {
		LogErrorDetail("[Vulkan][Init] Failed to create command pool");
	}
}

VulkanCommandPool::~VulkanCommandPool() {
	if (commandpool_) {
		vkDestroyCommandPool(device_, commandpool_, nullptr);
	}
}

} //namespace Driver::Vulkan