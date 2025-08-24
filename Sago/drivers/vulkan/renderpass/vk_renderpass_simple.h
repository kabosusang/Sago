#ifndef SG_VULKAN_RENDERPASS_SIMPLE_H
#define SG_VULKAN_RENDERPASS_SIMPLE_H
#include <volk.h>

#include "meta/traits/class_traits.h"
#include "vk_renderpass_base.h"


#include "drivers/vulkan/extensions/vk_swapchain.h"

namespace Driver::Vulkan {

class VulkanSimpleRenderPass : public RenderPassBase<VulkanSimpleRenderPass> {
	friend class RenderPassBase<VulkanSimpleRenderPass>;
	DEFINE_CLASS_NAME(SimpleRenderPass);

public:
	VulkanSimpleRenderPass(const VulkanDevice&, const VulkanSwapchain&);
	~VulkanSimpleRenderPass() noexcept;

private:
	void CreateRenderPassImpl();

private:
	const VulkanSwapchain& swapchain_;
	const VulkanDevice& device_;

};

} //namespace Driver::Vulkan

#endif