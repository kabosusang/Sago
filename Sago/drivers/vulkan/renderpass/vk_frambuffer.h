#ifndef SG_VULKAN_FRAMBUFFER_H
#define SG_VULKAN_FRAMBUFFER_H

#include <volk.h>
#include <cstdint>
#include <vector>

namespace Driver::Vulkan {

class VulkanFrameBuffer {
public:
	struct CreateInfo {
        VkDevice device = VK_NULL_HANDLE;
        VkRenderPass renderPass = VK_NULL_HANDLE;
        std::vector<VkImageView> attachments;
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t layers = 1;
        VkFramebufferCreateFlags flags = 0;
	};

public:
	VulkanFrameBuffer(const CreateInfo& info);
    VulkanFrameBuffer(CreateInfo&& info);
	~VulkanFrameBuffer();

    VulkanFrameBuffer(const VulkanFrameBuffer&) = delete;
    VulkanFrameBuffer& operator=(const VulkanFrameBuffer&) = delete;
    VulkanFrameBuffer(VulkanFrameBuffer&& other) noexcept;
    VulkanFrameBuffer& operator=(VulkanFrameBuffer&& other) noexcept;

    void ReCreate(const CreateInfo& info);
private:
	void CreateFrameBuffer();

private:
    CreateInfo createinfo_{};
	std::vector<VkFramebuffer> framebuffers_;
};

} //namespace Driver::Vulkan

#endif