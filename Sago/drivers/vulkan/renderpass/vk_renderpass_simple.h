#ifndef SG_VULKAN_RENDERPASS_SIMPLE_H
#define SG_VULKAN_RENDERPASS_SIMPLE_H
#include "vk_renderpass_base.h"
#include "meta/traits/class_traits.h"


namespace Driver::Vulkan {

class VulkanSimpleRenderPass :public RenderPassBase<VulkanSimpleRenderPass>{
    friend class RenderPassBase<VulkanSimpleRenderPass>;
	DEFINE_CLASS_NAME(VulkanSimplePipeline);
public:

};

} //namespace Driver::Vulkan

#endif