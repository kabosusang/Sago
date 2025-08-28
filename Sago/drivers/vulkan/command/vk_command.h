#ifndef SG_VULKAN_COMMAND_H
#define SG_VULKAN_COMMAND_H

#include <volk.h>

#include "drivers/vulkan/vk_device.h"

namespace Driver::Vulkan {

class VulkanCommand {
public:
	VulkanCommand(const VulkanInitializer&, const VulkanDevice&);
	~VulkanCommand();

	VulkanCommand(const VulkanCommand&) = delete;
	VulkanCommand& operator=(const VulkanCommand&) = delete;
	VulkanCommand(VulkanCommand&&) = delete;
	VulkanCommand& operator=(VulkanCommand&&) = delete;

	void BeginRecording(VkCommandBufferUsageFlags flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	void EndRecording();

	void submit(const std::vector<VkSemaphore>& waitSemaphores,
			const std::vector<VkPipelineStageFlags>& waitStages,
			const std::vector<VkSemaphore>& signalSemaphores,
			VkFence fence);

	bool IsRecording() { return isrecording_; }

	VkCommandBuffer getHandle() const { return commandbuffer_; }

private:
	void CreateCommandPool();
	void CreateCommandBuffer();

private:
	const VulkanInitializer& init_;
	const VulkanDevice& device_;

private:
	VkCommandPool commandpool_{};
	VkCommandBuffer commandbuffer_{};
	bool isrecording_{ false };
};

} //namespace Driver::Vulkan

#endif