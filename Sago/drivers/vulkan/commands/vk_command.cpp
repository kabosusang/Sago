#include "vk_command.h"

#include "core/io/log/log.h"
#include "drivers/vulkan/util/vk_queue_faimly.h"
#include <cstdint>

namespace Driver::Vulkan {
VulkanCommand::VulkanCommand(const VkPhysicalDevice psd, const VkDevice device, const VkQueue queue) :
		phydevice(psd), device_(device), queue_(queue) {
	CreateCommandPool();
	CreateCommandBuffer();
}

VulkanCommand::~VulkanCommand() {
	if (commandpool_) {
		vkDestroyCommandPool(device_, commandpool_, nullptr);
	}
}

void VulkanCommand::CreateCommandPool() {
	auto indice_graphy = FindIndice(Graphy{}, phydevice);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = indice_graphy.family_.value();

	if (vkCreateCommandPool(device_, &poolInfo, nullptr, &commandpool_) != VK_SUCCESS) {
		LogErrorDetail("[Vulkan][Init] Failed To Create CommandPool");
	}
}

void VulkanCommand::CreateCommandBuffer() {
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandpool_;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(device_, &allocInfo, &commandbuffer_) != VK_SUCCESS) {
		LogErrorDetail("[Vulkan][Init] Failed To Create CommandBuffer");
	}
}

void VulkanCommand::Reset() {
	vkResetCommandBuffer(commandbuffer_, 0);
}

void VulkanCommand::Reset(VkCommandBufferResetFlagBits flag) {
	vkResetCommandBuffer(commandbuffer_, flag);
}

void VulkanCommand::BeginRecording() {
	if (isrecording_) {
		LogErrorDetail("[Vulkan][Command] Failed To BeginRecording");
	}

	VkCommandBufferBeginInfo begin_info{};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(commandbuffer_, &begin_info) != VK_SUCCESS) {
		LogErrorDetail("[Vulkan][Command] Failed To BeginRecording");
	}
	isrecording_ = true;
}

void VulkanCommand::BeginRecording(VkCommandBufferUsageFlags flags) {
	if (isrecording_) {
		LogErrorDetail("[Vulkan][Command] Failed To BeginRecording");
	}

	VkCommandBufferBeginInfo begin_info{};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = flags;

	if (vkBeginCommandBuffer(commandbuffer_, &begin_info) != VK_SUCCESS) {
		LogErrorDetail("[Vulkan][Command] Failed To BeginRecording");
	}
	isrecording_ = true;
}

void VulkanCommand::EndRecording() {
	if (!isrecording_) {
		LogErrorDetail("[Vulkan][Command] Failed To EndRecording");
	}

	if (vkEndCommandBuffer(commandbuffer_) != VK_SUCCESS) {
		LogErrorDetail("[Vulkan][Command] Failed To EndRecording");
	}

	isrecording_ = false;
}

void VulkanCommand::Submit(const std::vector<VkSemaphore>& waitSemaphores,
		const std::vector<VkPipelineStageFlags>& waitStages,
		const std::vector<VkSemaphore>& signalSemaphores,
		VkFence fence) {
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandbuffer_;

	submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
	submitInfo.pWaitSemaphores = waitSemaphores.data();
	submitInfo.pWaitDstStageMask = waitStages.data();

	submitInfo.signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
	submitInfo.pSignalSemaphores = signalSemaphores.data();

	if (vkQueueSubmit(queue_, 1, &submitInfo, fence) != VK_SUCCESS) {
		LogErrorDetail("[Vulkan][Command] Failed To submit");
	}
}

void VulkanCommand::Present(VkQueue presentQueue,const std::vector<VkSemaphore>& signalSemaphores,
	VkSwapchainKHR swapchain,uint32_t imageindex) const {
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = signalSemaphores.size();
	presentInfo.pWaitSemaphores = signalSemaphores.data();

	VkSwapchainKHR swapChains[] = { swapchain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageindex;

	vkQueuePresentKHR(presentQueue, &presentInfo);
}

} //namespace Driver::Vulkan