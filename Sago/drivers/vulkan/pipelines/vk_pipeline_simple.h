#ifndef SG_VK_SIMPLE_PIPELINE_H
#define SG_VK_SIMPLE_PIPELINE_H
#include "vk_pipeline_base.h"

#include "drivers/vulkan/vk_device.h"

namespace Driver::Vulkan {

class VulkanSimplePipeline : public VulkanPipelineBase<VulkanSimplePipeline> {
public:
	VulkanSimplePipeline(const VulkanDevice&);
	~VulkanSimplePipeline() noexcept;

private:
	inline const char* GetPipelinName() { return "Simple_"; }
	void CreatePipelineImpl();
	VkShaderModule createShaderModule(const std::vector<char>&);

private:
	const VulkanDevice& device_;
};

} //namespace Driver::Vulkan

#endif