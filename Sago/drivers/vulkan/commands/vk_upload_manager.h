#ifndef SG_VULKAN_UPLOAD_MANAGER_H
#define SG_VULKAN_UPLOAD_MANAGER_H
#include "vk_command.h"
#include "vk_commandpool.h"
#include "vk_fence.h"
#include <drivers/vulkan/memory/vk_vma_allocator.h>
#include <drivers/vulkan/vk_device.h>
#include <memory>

namespace Driver::Vulkan {

class VulkanUploadManager {
public:
	VulkanUploadManager(const VulkanDevice&, uint32_t);

	void UploadBufferData(Memory::VulkanAllocator& allocator,
			const Memory::VulkanAllocator::Buffer& srcStaging,
			const Memory::VulkanAllocator::Buffer& dstDevice);

	void UploadMultipleBuffers(
			Memory::VulkanAllocator& allocator,
			const std::vector<std::pair<
					const Memory::VulkanAllocator::Buffer&, // srcStaging
					const Memory::VulkanAllocator::Buffer& // dstDevice
					>>& bufferPairs);

	void UploadBufferToImage(
			Memory::VulkanAllocator& allocator,
			const Memory::VulkanAllocator::Buffer& srcStaging,
			const Memory::VulkanAllocator::Image& dstImage,
			VkImageLayout finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			uint32_t width = 0, uint32_t height = 0, uint32_t mipLevels = 1);
private:
	void SubmitAndWait();

private:
	const VulkanDevice& device_;
	std::unique_ptr<VulkanCommandPool> upload_pool_;
	std::unique_ptr<VulkanCommand> upload_command_;
	std::unique_ptr<VulkanFence> upload_fence_;
};

} //namespace Driver::Vulkan

#endif