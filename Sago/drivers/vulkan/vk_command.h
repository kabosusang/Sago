#ifndef SG_VULKAN_COMMAND_H
#define SG_VULKAN_COMMAND_H

#include <volk.h>

#include "vk_device.h"
#include "vk_instance.h"

namespace Driver::Vulkan {

class VulkanCommand {
	VulkanCommand(const VulkanInitializer&,const VulkanDevice&);
	~VulkanCommand();

	VulkanCommand(const VulkanCommand&) = delete;
	VulkanCommand& operator=(const VulkanCommand&) = delete;
	VulkanCommand(VulkanCommand&&) = delete;
	VulkanCommand& operator=(VulkanCommand&&) = delete;

private:
    void CreateCommandPool();
    void CreateCommandBuffer();
    void RecordCommanndBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
private:
    const VulkanInitializer& init_;
    const VulkanDevice& device_;
private:
	VkCommandPool commandpool_;
    VkCommandBuffer commandbuffer_;
};

} //namespace Driver::Vulkan

#endif