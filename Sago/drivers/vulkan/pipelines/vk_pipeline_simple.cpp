#include "vk_pipeline_simple.h"

#include "drivers/vulkan/tools/vk_read_shaders.h"
#include <cmath>

namespace Driver::Vulkan {

VulkanSimplePipeline::VulkanSimplePipeline(const VulkanDevice& device) :
		device_(device) {
	CreatePipelineImpl();
}

VulkanSimplePipeline::~VulkanSimplePipeline() noexcept {
	shader_manager_.Release(GetDevice(device_));
}

void VulkanSimplePipeline::CreatePipelineImpl() {
	// auto vertFuture = Tools::ReadShaderFileAsync("shaders/vert.spv");
	// auto fragFuture = Tools::ReadShaderFileAsync("shaders/frag.spv");

	// std::vector<char> vertCode = vertFuture.get();
	// std::vector<char> fragCode = fragFuture.get();

	// VkShaderModule vertShaderModule = createShaderModule(vertCode);
	// VkShaderModule fragShaderModule = createShaderModule(fragCode);

	auto vert = shader_manager_.LoadShaderModuleAsync(GetDevice(device_), "triangle_vert.spv");
	auto frag = shader_manager_.LoadShaderModuleAsync(GetDevice(device_), "triangle_frag.spv");

	LogInfo("运行{}",vert.GetNative());
	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vert.GetValue();
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = frag.GetValue();
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
}


} //namespace Driver::Vulkan