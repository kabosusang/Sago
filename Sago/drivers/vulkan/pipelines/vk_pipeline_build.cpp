#include "vk_pipeline_build.h"
#include <vector>

namespace Driver::Vulkan {

PipelineBuilder::PipelineBuilder(const VulkanDevice& device, const VulkanSwapchain& swapchain) :
		device_(device), swapchain_(swapchain) {
	shader_stages_.reserve(2);
}

template <ShaderStrategyType Strategy>
VkPipelineShaderStageCreateInfo PipelineBuilder::CreateShaderStage(VkShaderModule module, std::string_view entry) const {
	VkPipelineShaderStageCreateInfo stageInfo{};
	stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stageInfo.stage = Strategy::stage;
	stageInfo.module = module;
	stageInfo.pName = entry.data();
	return stageInfo;
}

template <typename... Strategies>
PipelineBuilder& PipelineBuilder::SetShaderModule(const ShaderModuleInfo<Strategies>&... modules) {
	shader_stages_.emplace_back((CreateShaderStage<Strategies>(modules.module_), ...));
	return *this;
}
//auto vertShader = ShaderModuleInfo<ShaderStrategy::Vert>{vertexModule};
//builder.SetShaderModule(vertShader);

PipelineBuilder& PipelineBuilder::SetVertexInputInfo(const VkPipelineVertexInputStateCreateInfo& info) {
	vertex_input_info_ = info;
	vertex_input_info_.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	return *this;
}

PipelineBuilder& PipelineBuilder::SetInputAssemblyInfo(const VkPipelineInputAssemblyStateCreateInfo& info) {
	input_assembly_info_ = info;
	input_assembly_info_.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	return *this;
}

PipelineBuilder& PipelineBuilder::SetDynamicStateInfo(const std::vector<VkDynamicState>& dynamic) {
	dynamic_state_info_.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state_info_.dynamicStateCount = static_cast<uint32_t>(dynamic.size());
	dynamic_state_info_.pDynamicStates = dynamic.data();
	return *this;
}

PipelineBuilder& PipelineBuilder::SetViewports(const std::vector<VkViewport>& viewports) {
	viewports_ = viewports;
	viewport_state_info_.viewportCount = static_cast<uint32_t>(viewports_.size());
	viewport_state_info_.pViewports = viewports_.empty() ? nullptr : viewports_.data();
	return *this;
}

PipelineBuilder& PipelineBuilder::SetScissors(const std::vector<VkRect2D>& scissors) {
	scissors_ = scissors;
	viewport_state_info_.scissorCount = static_cast<uint32_t>(scissors_.size());
	viewport_state_info_.pScissors = scissors_.empty() ? nullptr : scissors_.data();
	return *this;
}

PipelineBuilder& PipelineBuilder::SetViewportStateInfo(const VkPipelineViewportStateCreateInfo& info) {
	viewport_state_info_.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	return *this;
}

PipelineBuilder& PipelineBuilder::SetRasterizationInfo(const VkPipelineRasterizationStateCreateInfo& info) {
	rasterization_info_ = info;
	rasterization_info_.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	return *this;
}

PipelineBuilder& PipelineBuilder::SetMultisampleInfo(const VkPipelineMultisampleStateCreateInfo& info) {
	multisample_info_ = info;
	multisample_info_.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	return *this;
}

PipelineBuilder& PipelineBuilder::SetColorBlendInfo(const std::vector<VkPipelineColorBlendAttachmentState>& attachments, const VkPipelineColorBlendStateCreateInfo& info) {
	color_blend_info_ = info;
	color_blend_info_.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	color_blend_info_.attachmentCount = static_cast<uint32_t>(attachments.size());
	color_blend_info_.pAttachments = attachments.data();
	return *this;
}

PipelineBuilder& PipelineBuilder::SetPipelineLayout(VkPipelineLayout layout) {
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0; // Optional
	pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

	if (vkCreatePipelineLayout(GetDevice(device_), &pipelineLayoutInfo, nullptr, &pipeline_layout_) != VK_SUCCESS) {
		LogErrorDetaill("[Vulkan][Pipeline]: Faile to Create Pileline Layout ");
	}
	return *this;
}

VkPipeline PipelineBuilder::Build() {
	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = static_cast<uint32_t>(shader_stages_.size());
	pipelineInfo.pStages = shader_stages_.data();

	pipelineInfo.pVertexInputState = &vertex_input_info_;
	pipelineInfo.pInputAssemblyState = &input_assembly_info_;
	pipelineInfo.pViewportState = &viewport_state_info_;
	pipelineInfo.pRasterizationState = &rasterization_info_;
	pipelineInfo.pMultisampleState = &multisample_info_;
	pipelineInfo.pDepthStencilState = nullptr; // Optional
	pipelineInfo.pColorBlendState = &color_blend_info_;
	pipelineInfo.pDynamicState = &dynamic_state_info_;
	pipelineInfo.layout = pipeline_layout_;
	pipelineInfo.renderPass = render_pass_;
	pipelineInfo.subpass = 0;

	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

	VkPipeline graphicsPipeline;
	if (vkCreateGraphicsPipelines(GetDevice(device_), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
		LogErrorDetaill("[Vulkan][Pipeline]: Faile to Create Pileline");
	}
    return graphicsPipeline;
}

} //namespace Driver::Vulkan