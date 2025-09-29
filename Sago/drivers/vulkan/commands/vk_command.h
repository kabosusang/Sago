#ifndef SG_VULKAN_COMMAND_H
#define SG_VULKAN_COMMAND_H

#include <volk.h>
#include <cstdint>

#include "drivers/vulkan/vk_device.h"

namespace Driver::Vulkan {

class VulkanCommand {
public:
	VulkanCommand() = default;
	VulkanCommand(const VkDevice, const VkCommandPool pool, const VkQueue);
	~VulkanCommand();

	VulkanCommand(const VulkanCommand&) = delete;
	VulkanCommand& operator=(const VulkanCommand&) = delete;
	VulkanCommand(VulkanCommand&&) = delete;
	VulkanCommand& operator=(VulkanCommand&&) = delete;

	void Reset(VkCommandBufferResetFlagBits flag);
	void Reset();

	void BeginRecording(VkCommandBufferUsageFlags flags);
	void BeginRecording();
	void EndRecording();

	void Submit(const std::vector<VkSemaphore>& waitSemaphores,
			const std::vector<VkPipelineStageFlags>& waitStages,
			const std::vector<VkSemaphore>& signalSemaphores,
			VkFence fence);

	std::pair<VkResult, std::string> Present(VkQueue presentQueue, const std::vector<VkSemaphore>& signalSemaphores,
			VkSwapchainKHR swapchain, uint32_t imageindex) const;

	bool IsRecording() { return isrecording_; }
	VkCommandBuffer getHandle() const { return commandbuffer_; }
	operator VkCommandBuffer() const { return commandbuffer_; }

private:
	void CreateCommandBuffer();

private:
	const VkPhysicalDevice phydevice{};
	const VkDevice device_{};
	const VkQueue queue_{};

private:
	VkCommandPool commandpool_{};
	VkCommandBuffer commandbuffer_{};
	bool isrecording_{ false };
};

} //namespace Driver::Vulkan

#endif