#include "vk_pipeline_simple.h"

#include "drivers/vulkan/tools/vk_read_shaders.h"

namespace Driver::Vulkan {

VulkanSimplePipeline::VulkanSimplePipeline(const VulkanDevice& device) :
		device_(device) {
	LogInfo("[Vulkan][Pipeline] Create Simple Pipeline");
	CreatePipelineImpl();
}

VulkanSimplePipeline::~VulkanSimplePipeline() noexcept {
}

void VulkanSimplePipeline::CreatePipelineImpl() {
	auto vertShaderCode = Tools::ReadShaderFile("shaders/vert.spv");
	auto fragShaderCode = Tools::ReadShaderFile("shaders/frag.spv");

	VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	vkDestroyShaderModule(GetDevice(device_), fragShaderModule, nullptr);
	vkDestroyShaderModule(GetDevice(device_), vertShaderModule, nullptr);
}

VkShaderModule VulkanSimplePipeline::createShaderModule(const std::vector<char>& code) {
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(GetDevice(device_), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		LogErrorDetaill("[Vulkan][Pipeline] Fail To Create ShaderModule");
	}

	return shaderModule;
}

} //namespace Driver::Vulkan