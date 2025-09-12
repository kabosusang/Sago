#ifndef SG_VK_SIMPLE_PIPELINE_H
#define SG_VK_SIMPLE_PIPELINE_H
#include "vk_pipeline_base.h"

#include "drivers/vulkan/extensions/vk_swapchain.h"
#include "drivers/vulkan/renderpass/vk_renderpass_simple.h"
#include "drivers/vulkan/vk_device.h"
#include "vk_pipeline_build.h"

#include "meta/traits/class_traits.h"

#include "shaders/vk_shaders_manager.h"

namespace Driver::Vulkan {

class VulkanSimplePipeline : public VulkanPipelineBase<VulkanSimplePipeline> {
	friend class VulkanPipelineBase<VulkanSimplePipeline>;
	DEFINE_CLASS_NAME(SimplePipeline);

public:
	VulkanSimplePipeline(const VulkanDevice&, const VulkanSwapchain&, const VulkanSimpleRenderPass&);
	~VulkanSimplePipeline() noexcept;
private:
	void CreatePipelineImpl();

private:
	void SetDefaultStatus(PipelineBuilder& builder) const;

private:
	const VulkanDevice& device_;
	const VulkanSwapchain& swapchain_;
	const VulkanSimpleRenderPass& renderpass_;

private:
	Shader::VulkanShaderManager shader_manager_{ "Assets/Shaders/simple_pipeline/" };
};

} //namespace Driver::Vulkan

#endif