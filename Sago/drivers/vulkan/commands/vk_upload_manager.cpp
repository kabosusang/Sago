#include "vk_upload_manager.h"
#include "vk_fence.h"
#include <cstdint>

#include "drivers/vulkan/commands/vk_commandbuild.h"

namespace Driver::Vulkan {
using namespace Driver::Vulkan::Memory;
VulkanUploadManager::VulkanUploadManager(const VulkanDevice& device, uint32_t queueFamilyIndex) :
		device_(device) {
	upload_pool_ = std::make_unique<VulkanCommandPool>(
			device.GetDevice(), queueFamilyIndex);
	upload_command_ = upload_pool_->CreateCommand(device.GetGraphyciQueue());
	upload_fence_ = std::make_unique<VulkanFence>(device.GetDevice(), 0);
}

void VulkanUploadManager::UploadBufferData(
		VulkanAllocator& allocator,
		const VulkanAllocator::Buffer& srcStaging,
		const VulkanAllocator::Buffer& dstDevice) {
	upload_command_->Reset();
	upload_command_->BeginRecording();
	{
		CommandBuilder builder(*upload_command_);
		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = srcStaging.GetSize();

		builder.CopyBuffer(
				srcStaging.buffer,
				dstDevice.buffer,
				{ copyRegion });
	}
	upload_command_->EndRecording();

	SubmitAndWait();
}

void VulkanUploadManager::UploadMultipleBuffers(
		Memory::VulkanAllocator& allocator,
		const std::vector<std::pair<
				const Memory::VulkanAllocator::Buffer&, // srcStaging
				const Memory::VulkanAllocator::Buffer& // dstDevice
				>>& bufferPairs) {
	upload_command_->Reset();
	upload_command_->BeginRecording();
	{
		CommandBuilder builder(*upload_command_);

		for (const auto& [src, dst] : bufferPairs) {
			VkBufferCopy copyRegion{};
			copyRegion.srcOffset = 0;
			copyRegion.dstOffset = 0;
			copyRegion.size = src.GetSize();

			builder.CopyBuffer(src.buffer, dst.buffer, { copyRegion });
		}
	}
	upload_command_->EndRecording();
	SubmitAndWait();
}

void VulkanUploadManager::SubmitAndWait() {
	upload_fence_->reset();

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer = upload_command_->getHandle();
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(device_.GetGraphyciQueue(), 1, &submitInfo, *upload_fence_);
	upload_fence_->wait(UINT64_MAX);
}

void VulkanUploadManager::UploadBufferToImage(
		Memory::VulkanAllocator& allocator,
		const Memory::VulkanAllocator::Buffer& srcStaging,
		const Memory::VulkanAllocator::Image& dstImage,
		VkImageLayout finalLayout,
		uint32_t width, uint32_t height, uint32_t mipLevels) {
	upload_command_->Reset();
	upload_command_->BeginRecording();
	{
		CommandBuilder builder(*upload_command_);

		// Transition
		builder.TransitionImageLayout(
				dstImage.image,
				VK_FORMAT_UNDEFINED, //image format
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				mipLevels);

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { width, height, 1 };

		builder.CopyBufferToImage(
				srcStaging.buffer,
				dstImage.image,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				{ region });

		builder.TransitionImageLayout(
				dstImage.image,
				VK_FORMAT_UNDEFINED,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				finalLayout,
				mipLevels);
	}
	upload_command_->EndRecording();

	SubmitAndWait();
}

} //namespace Driver::Vulkan