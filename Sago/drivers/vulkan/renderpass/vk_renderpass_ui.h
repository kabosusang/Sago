#ifndef SG_VULKAN_RENDERPASS_UI_H
#define SG_VULKAN_RENDERPASS_UI_H
#include <volk.h>

#include "meta/traits/class_traits.h"
#include "vk_renderpass_base.h"

namespace Driver::Vulkan {

class VulkanUIRenderPass : public RenderPassBase<VulkanUIRenderPass> {
	friend class RenderPassBase<VulkanUIRenderPass>;
	DEFINE_CLASS_NAME(SimpleRenderPass);

public:
	VulkanUIRenderPass(VkDevice device, VkFormat format);
	~VulkanUIRenderPass() noexcept;
private:
	void CreateRenderPassImpl();
private:
	VkFormat format_;
    VkDevice device_;
};

} //namespace Driver::Vulkan

#endif