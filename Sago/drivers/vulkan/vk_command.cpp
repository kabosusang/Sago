#include "vk_command.h"

#include "core/io/log/log.h"
#include "util/vk_queue_faimly.h"
#include "vk_device.h"

namespace Driver::Vulkan {
VulkanCommand::VulkanCommand(const VulkanInitializer& init, const VulkanDevice& device) :
		init_(init), device_(device) {
	CreateCommandPool();
	CreateCommandBuffer();
}

VulkanCommand::~VulkanCommand() {
	if (commandpool_) {
		vkDestroyCommandPool(GetDevice(device_), commandpool_, nullptr);
	}
}

void VulkanCommand::CreateCommandPool() {
	auto indice_graphy = FindIndice(Graphy{}, GetPhysicalDevice(init_));

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = indice_graphy.family_.value();

	if (vkCreateCommandPool(GetDevice(device_), &poolInfo, nullptr, &commandpool_) != VK_SUCCESS) {
		LogErrorDetaill("[Vulkan][Init] Failed To Create CommandPool");
	}
}

void VulkanCommand::CreateCommandBuffer() {
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandpool_;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(GetDevice(device_), &allocInfo, &commandbuffer_) != VK_SUCCESS) {
		LogErrorDetaill("[Vulkan][Init] Failed To Create CommandBuffer");
	}
}

void VulkanCommand::RecordCommanndBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        LogErrorDetaill("[Vulkan][Command] Failed To begin recording command buffer!");
    }
}


} //namespace Driver::Vulkan