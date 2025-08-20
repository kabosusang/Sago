#ifndef SG_VK_SIMPLE_PIPELINE_H
#define SG_VK_SIMPLE_PIPELINE_H
#include "vk_pipeline_base.h"

#include "drivers/vulkan/vk_device.h"
#include "drivers/vulkan/extensions/vk_swapchain.h"

#include "meta/traits/class_traits.h"

#include "shaders/vk_shaders_manager.h"

namespace Driver::Vulkan {

class VulkanSimplePipeline : public VulkanPipelineBase<VulkanSimplePipeline> {
	friend class VulkanPipelineBase<VulkanSimplePipeline>;
	DEFINE_CLASS_NAME(VulkanSimplePipeline);
public:
	VulkanSimplePipeline(const VulkanDevice&,const VulkanSwapchain&);
	~VulkanSimplePipeline() noexcept;
private:
	void CreatePipelineImpl();
	VkShaderModule createShaderModule(const std::vector<char>&);

private:
	void CreateVertexInputAndAssembly();
	void SetViewPortAndScissor();
	void SetRasterization();
	void SetMultisample();
	void SetDepthAndStencil();

private:
	const VulkanDevice& device_;
	const VulkanSwapchain& swapchain_;
private:
	Shader::VulkanShaderManager shader_manager_{"Assets/Shaders/simple_pipeline/"};
	VkPipelineLayout pipeline_layout_;
};

} //namespace Driver::Vulkan

#endif