#ifndef SG_VULKAN_COMMANDBUILD_H
#define SG_VULKAN_COMMANDBUILD_H
#include "vk_command.h"

namespace Driver::Vulkan {

class CommandBuilder {
public:
	explicit CommandBuilder(VulkanCommand& command) :
			command_(command) {
		if (!command_.IsRecording()) {
			command_.BeginRecording();
		}
		cmdBuffer_ = command_.getHandle();
	}

	CommandBuilder(const CommandBuilder&) = delete;
	CommandBuilder& operator=(const CommandBuilder&) = delete;
	CommandBuilder& BeginRenderPass(VkRenderPass renderPass, VkFramebuffer framebuffer,
			VkExtent2D extent,
			const std::vector<VkClearValue>& clearValues = {});
	CommandBuilder& EndRenderPass();

	CommandBuilder& BindGraphicsPipeline(VkPipeline pipeline);
	CommandBuilder& BindComputePipeline(VkPipeline pipeline);

	CommandBuilder& SetViewport(float x, float y, float width, float height,
			float minDepth = 0.0f, float maxDepth = 1.0f);
	CommandBuilder& SetViewport(VkExtent2D extent);

	CommandBuilder& SetScissor(int32_t offsetX, int32_t offsetY,
			uint32_t width, uint32_t height);

	CommandBuilder& SetScissor(VkExtent2D extent);

	//Draw
	CommandBuilder& Draw(uint32_t vertexCount, uint32_t instanceCount = 1,
			uint32_t firstVertex = 0, uint32_t firstInstance = 0);

	CommandBuilder& DrawIndexed(uint32_t indexCount, uint32_t instanceCount = 1,
			uint32_t firstIndex = 0, int32_t vertexOffset = 0,
			uint32_t firstInstance = 0);

	CommandBuilder& BindVertexBuffers(uint32_t firstBinding,
			const std::vector<VkBuffer>& buffers,
			const std::vector<VkDeviceSize>& offsets);

	CommandBuilder& BindIndexBuffer(VkBuffer buffer, VkDeviceSize offset,
			VkIndexType indexType);

	CommandBuilder& BindDescriptorSets(VkPipelineBindPoint pipelineBindPoint,
			VkPipelineLayout layout, uint32_t firstSet,
			const std::vector<VkDescriptorSet>& descriptorSets,
			const std::vector<uint32_t>& dynamicOffsets = {});

	//Push Constants
	template <typename T>
	CommandBuilder& PushConstants(VkPipelineLayout layout, VkShaderStageFlags stageFlags,
			uint32_t offset, const T& data);

	CommandBuilder& CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer,
			const std::vector<VkBufferCopy>& regions);

	CommandBuilder& CopyBufferToImage(VkBuffer srcBuffer, VkImage dstImage,
			VkImageLayout dstImageLayout,
			const std::vector<VkBufferImageCopy>& regions);

	CommandBuilder& PipelineBarrier(VkPipelineStageFlags srcStageMask,
			VkPipelineStageFlags dstStageMask,
			VkDependencyFlags dependencyFlags,
			const std::vector<VkMemoryBarrier>& memoryBarriers = {},
			const std::vector<VkBufferMemoryBarrier>& bufferMemoryBarriers = {},
			const std::vector<VkImageMemoryBarrier>& imageMemoryBarriers = {});

	CommandBuilder& Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);

	//Transtion
	CommandBuilder& TransitionImageLayout(VkImage image, VkFormat format,
			VkImageLayout oldLayout, VkImageLayout newLayout,
			uint32_t mipLevels = 1, uint32_t layerCount = 1);

	bool IsInRenderPass() const { return inRenderPass_; }
	VkCommandBuffer GetCommandBuffer() const { return cmdBuffer_; }

private:
	void CheckInRenderPass() const;

private:
	VulkanCommand& command_;
	VkCommandBuffer cmdBuffer_;
	bool inRenderPass_{ false };
};

} //namespace Driver::Vulkan

#endif