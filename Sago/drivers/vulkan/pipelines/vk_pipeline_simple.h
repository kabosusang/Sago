#ifndef SG_VK_SIMPLE_PIPELINE_H
#define SG_VK_SIMPLE_PIPELINE_H
#include "vk_pipeline_base.h"

#include "drivers/vulkan/vk_device.h"
#include "meta/traits/class_traits.h"


namespace Driver::Vulkan {

class VulkanSimplePipeline : public VulkanPipelineBase<VulkanSimplePipeline> {
	friend class VulkanPipelineBase<VulkanSimplePipeline>;
	DEFINE_CLASS_NAME(VulkanSimplePipeline);
public:
	VulkanSimplePipeline(const VulkanDevice&);
	~VulkanSimplePipeline() noexcept;
private:
	void CreatePipelineImpl();
	VkShaderModule createShaderModule(const std::vector<char>&);

private:
	const VulkanDevice& device_;
	
};

} //namespace Driver::Vulkan

#endif