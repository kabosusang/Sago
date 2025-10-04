#include "vk_commandbuild.h"

#include "drivers/vulkan/util/vk_default.h"
#include "core/io/log/log.h"

namespace Driver::Vulkan {

void CommandBuilder::CheckInRenderPass() const {
	if (!inRenderPass_) {
		LogErrorDetail("[Vulkan][CommandBuilder] Command requires render pass to be active!");
	}
}

CommandBuilder& CommandBuilder::BeginRenderPass(VkRenderPass renderPass, VkFramebuffer framebuffer,
		VkExtent2D extent,
		const std::vector<VkClearValue>& clearValues) {
	if (inRenderPass_) {
		LogErrorDetail("[Vulkan][CommandBuilder] Already in render pass!");
		return *this;
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = framebuffer;
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = extent;

	if (clearValues.empty()) {
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &Default::Colors::Black;
	} else {
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();
	}

	vkCmdBeginRenderPass(cmdBuffer_, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	inRenderPass_ = true;
	return *this;
}

CommandBuilder& CommandBuilder::EndRenderPass() {
	if (!inRenderPass_) {
		LogErrorDetail("[Vulkan][CommandBuilder] Not in render pass!");
		return *this;
	}

	vkCmdEndRenderPass(cmdBuffer_);
	inRenderPass_ = false;
	return *this;
}

//Pipeline
CommandBuilder& CommandBuilder::BindGraphicsPipeline(VkPipeline pipeline) {
	CheckInRenderPass();
	vkCmdBindPipeline(cmdBuffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	return *this;
}

CommandBuilder& CommandBuilder::BindComputePipeline(VkPipeline pipeline) {
	CheckInRenderPass();
	vkCmdBindPipeline(cmdBuffer_, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
	return *this;
}

//Viewport And Scissors

CommandBuilder& CommandBuilder::SetViewport(float x, float y, float width, float height,
		float minDepth, float maxDepth) {
	CheckInRenderPass();
	VkViewport viewport{ x, y, width, height, minDepth, maxDepth };
	vkCmdSetViewport(cmdBuffer_, 0, 1, &viewport);
	return *this;
}

CommandBuilder& CommandBuilder::SetViewport(VkExtent2D extent) {
	return SetViewport(0.0f, 0.0f,
			static_cast<float>(extent.width),
			static_cast<float>(extent.height));
}

CommandBuilder& CommandBuilder::SetScissor(int32_t offsetX, int32_t offsetY,
		uint32_t width, uint32_t height) {
	CheckInRenderPass();
	VkRect2D scissor{ { offsetX, offsetY }, { width, height } };
	vkCmdSetScissor(cmdBuffer_, 0, 1, &scissor);
	return *this;
}

CommandBuilder& CommandBuilder::SetScissor(VkExtent2D extent) {
	return SetScissor(0, 0, extent.width, extent.height);
}

//Draw
CommandBuilder& CommandBuilder::Draw(uint32_t vertexCount, uint32_t instanceCount,
		uint32_t firstVertex, uint32_t firstInstance) {
	CheckInRenderPass();
	vkCmdDraw(cmdBuffer_, vertexCount, instanceCount, firstVertex, firstInstance);
	return *this;
}

CommandBuilder& CommandBuilder::DrawIndexed(uint32_t indexCount, uint32_t instanceCount,
		uint32_t firstIndex, int32_t vertexOffset,
		uint32_t firstInstance) {
	CheckInRenderPass();
	vkCmdDrawIndexed(cmdBuffer_, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
	return *this;
}

CommandBuilder& CommandBuilder::BindVertexBuffers(uint32_t firstBinding,
		const std::vector<VkBuffer>& buffers,
		const std::vector<VkDeviceSize>& offsets) {
	CheckInRenderPass();
	vkCmdBindVertexBuffers(cmdBuffer_, firstBinding,
			static_cast<uint32_t>(buffers.size()),
			buffers.data(), offsets.data());
	return *this;
}

CommandBuilder& CommandBuilder::BindIndexBuffer(VkBuffer buffer, VkDeviceSize offset,
		VkIndexType indexType) {
	CheckInRenderPass();
	vkCmdBindIndexBuffer(cmdBuffer_, buffer, offset, indexType);
	return *this;
}

CommandBuilder& CommandBuilder::BindDescriptorSets(VkPipelineBindPoint pipelineBindPoint,
		VkPipelineLayout layout, uint32_t firstSet,
		const std::vector<VkDescriptorSet>& descriptorSets,
		const std::vector<uint32_t>& dynamicOffsets) {
	vkCmdBindDescriptorSets(cmdBuffer_, pipelineBindPoint, layout, firstSet,
			static_cast<uint32_t>(descriptorSets.size()),
			descriptorSets.data(),
			static_cast<uint32_t>(dynamicOffsets.size()),
			dynamicOffsets.data());
	return *this;
}

template <typename T>
CommandBuilder& CommandBuilder::PushConstants(VkPipelineLayout layout, VkShaderStageFlags stageFlags,
		uint32_t offset, const T& data) {
	vkCmdPushConstants(cmdBuffer_, layout, stageFlags, offset, sizeof(T), &data);
	return *this;
}

//Buffer Input
CommandBuilder& CommandBuilder::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer,
		const std::vector<VkBufferCopy>& regions) {
	vkCmdCopyBuffer(cmdBuffer_, srcBuffer, dstBuffer,
			static_cast<uint32_t>(regions.size()), regions.data());
	return *this;
}

CommandBuilder& CommandBuilder::CopyBufferToImage(VkBuffer srcBuffer, VkImage dstImage,
		VkImageLayout dstImageLayout,
		const std::vector<VkBufferImageCopy>& regions) {
	vkCmdCopyBufferToImage(cmdBuffer_, srcBuffer, dstImage, dstImageLayout,
			static_cast<uint32_t>(regions.size()), regions.data());
	return *this;
}

CommandBuilder& CommandBuilder::PipelineBarrier(VkPipelineStageFlags srcStageMask,
		VkPipelineStageFlags dstStageMask,
		VkDependencyFlags dependencyFlags,
		const std::vector<VkMemoryBarrier>& memoryBarriers,
		const std::vector<VkBufferMemoryBarrier>& bufferMemoryBarriers,
		const std::vector<VkImageMemoryBarrier>& imageMemoryBarriers) {
	vkCmdPipelineBarrier(cmdBuffer_, srcStageMask, dstStageMask, dependencyFlags,
			static_cast<uint32_t>(memoryBarriers.size()), memoryBarriers.data(),
			static_cast<uint32_t>(bufferMemoryBarriers.size()), bufferMemoryBarriers.data(),
			static_cast<uint32_t>(imageMemoryBarriers.size()), imageMemoryBarriers.data());
	return *this;
}

// Dispatch
CommandBuilder& CommandBuilder::Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) {
	vkCmdDispatch(cmdBuffer_, groupCountX, groupCountY, groupCountZ);
	return *this;
}

// ImageLayout
CommandBuilder& CommandBuilder::TransitionImageLayout(VkImage image, VkFormat format,
		VkImageLayout oldLayout, VkImageLayout newLayout,
		uint32_t mipLevels, uint32_t layerCount) {
	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = mipLevels;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = layerCount;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	} else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	} else {
		throw std::invalid_argument("Unsupported layout transition!");
	}

	return PipelineBarrier(sourceStage, destinationStage, 0, {}, {}, { barrier });
}

} //namespace Driver::Vulkan
