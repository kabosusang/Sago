#include "vk_frambuffer.h"

#include "core/io/log/log.h"

namespace Driver::Vulkan {

VulkanFrameBuffer::VulkanFrameBuffer(const CreateInfo& info) :
		createinfo_(info) {
	framebuffers_.resize(createinfo_.attachments.size());
	CreateFrameBuffer();
}

VulkanFrameBuffer::VulkanFrameBuffer(CreateInfo&& info) :
		createinfo_(std::move(info)) {
	framebuffers_.resize(createinfo_.attachments.size());
	CreateFrameBuffer();
}

VulkanFrameBuffer::~VulkanFrameBuffer() {
	for (auto framebuffer : framebuffers_) {
		vkDestroyFramebuffer(createinfo_.device, framebuffer, nullptr);
	}
}

void VulkanFrameBuffer::CreateFrameBuffer() {
	if (createinfo_.attachments.empty()) {
		LogWarring("[Vulkan][FrameBuffer] No Attachment For CreateFrameBuffers");
		return;
	}

	for (auto i = 0; i < createinfo_.attachments.size(); i++) {
		VkImageView attachments[] = {
			createinfo_.attachments[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = createinfo_.renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = createinfo_.width;
		framebufferInfo.height = createinfo_.height;
		framebufferInfo.layers = createinfo_.layers;
		framebufferInfo.flags = createinfo_.flags;

		if (vkCreateFramebuffer(createinfo_.device, &framebufferInfo, nullptr, &framebuffers_[i]) != VK_SUCCESS) [[unlikely]]{
			LogErrorDetail("[Vulkan][Init] Failed to Create FrameBuffer");
		}
	}
}

void VulkanFrameBuffer::CleanFrameBuffers() {
	for (auto framebuffer : framebuffers_) {
		vkDestroyFramebuffer(createinfo_.device, framebuffer, nullptr);
	}
}

void VulkanFrameBuffer::ReCreateSwapChain(){
	CreateFrameBuffer();
}

} //namespace Driver::Vulkan