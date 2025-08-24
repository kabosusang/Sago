#include "vk_pipeline_build.h"
#include <cstdint>
#include <vector>

namespace Driver::Vulkan {

PipelineBuilder::PipelineBuilder(const VulkanDevice& device) :
		device_(device) {
	shader_stages_.reserve(2);

	vertex_input_info_.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	input_assembly_info_.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	viewport_state_info_.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	rasterization_info_.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	multisample_info_.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	color_blend_info_.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	dynamic_state_info_.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
}

// template <ShaderStrategyType Strategy>
// VkPipelineShaderStageCreateInfo PipelineBuilder::CreateShaderStage(VkShaderModule module, std::string_view entry) const {
// 	VkPipelineShaderStageCreateInfo stageInfo{};
// 	stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
// 	stageInfo.stage = Strategy::stage;
// 	stageInfo.module = module;
// 	stageInfo.pName = entry.data();
// 	return stageInfo;
// }

// template <typename... Strategies>
// PipelineBuilder& PipelineBuilder::SetShaderModule(const ShaderModuleInfo<Strategies>&... modules) {
// 	shader_stages_.clear();
// 	(shader_stages_.push_back(CreateShaderStage<Strategies>(modules.module_, EngineShaderEntry)), ...);
// 	return *this;
// }
//auto vertShader = ShaderModuleInfo<ShaderStrategy::Vert>{vertexModule};
//builder.SetShaderModule(vertShader);

PipelineBuilder& PipelineBuilder::SetVertexInputInfo() {
	vertex_input_info_.vertexBindingDescriptionCount = 0;
	vertex_input_info_.pVertexBindingDescriptions = nullptr; // Optional
	vertex_input_info_.vertexAttributeDescriptionCount = 0;
	vertex_input_info_.pVertexAttributeDescriptions = nullptr; // Optional
	return *this;
}

PipelineBuilder& PipelineBuilder::SetVertexInputInfo(const std::vector<VkVertexInputBindingDescription>& binds, const std::vector<VkVertexInputAttributeDescription>& attributes) {
	vertex_input_info_.vertexBindingDescriptionCount = static_cast<uint32_t>(binds.size());
	vertex_input_info_.pVertexBindingDescriptions = binds.data();
	vertex_input_info_.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size());
	vertex_input_info_.pVertexAttributeDescriptions = attributes.data();
	return *this;
}

PipelineBuilder& PipelineBuilder::SetInputAssemblyInfo(const VkPipelineInputAssemblyStateCreateInfo& info) {
	input_assembly_info_ = info;
	return *this;
}

PipelineBuilder& PipelineBuilder::SetDynamicStateInfo(const std::vector<VkDynamicState>& dynamic) {
	dynamics_ = dynamic;
	dynamic_state_info_.dynamicStateCount = static_cast<uint32_t>(dynamics_.size());
	dynamic_state_info_.pDynamicStates = dynamics_.data();
	return *this;
}

PipelineBuilder& PipelineBuilder::SetDynamicStateInfo(std::vector<VkDynamicState>&& dynamic) {
	dynamics_ = std::move(dynamic);
	dynamic_state_info_.dynamicStateCount = static_cast<uint32_t>(dynamics_.size());
	dynamic_state_info_.pDynamicStates = dynamics_.data();
	return *this;
}

PipelineBuilder& PipelineBuilder::SetViewports(const std::vector<VkViewport>& viewports) {
	viewports_ = viewports;
	viewport_state_info_.viewportCount = static_cast<uint32_t>(viewports_.size());
	viewport_state_info_.pViewports = viewports_.empty() ? nullptr : viewports_.data();
	return *this;
}

PipelineBuilder& PipelineBuilder::SetViewports(std::vector<VkViewport>&& viewports) {
	viewports_ = std::move(viewports);
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

PipelineBuilder& PipelineBuilder::SetScissors(std::vector<VkRect2D>&& scissors) {
	scissors_ = std::move(scissors);
	viewport_state_info_.scissorCount = static_cast<uint32_t>(scissors_.size());
	viewport_state_info_.pScissors = scissors_.empty() ? nullptr : scissors_.data();
	return *this;
}


PipelineBuilder& PipelineBuilder::SetViewportStateInfo() {
	return *this;
}

PipelineBuilder& PipelineBuilder::SetViewportStateInfo(const VkPipelineViewportStateCreateInfo& info) {
	return *this;
}

PipelineBuilder& PipelineBuilder::SetRasterizationInfo(const VkPipelineRasterizationStateCreateInfo& info) {
	rasterization_info_ = info;
	return *this;
}

PipelineBuilder& PipelineBuilder::SetMultisampleInfo(const std::vector<VkSampleMask>& masks,const VkPipelineMultisampleStateCreateInfo& info) {
	samplemask_ = masks;
	multisample_info_ = info;
	if (masks.empty()){
		multisample_info_.pSampleMask = nullptr;
	}else{
		multisample_info_.pSampleMask = samplemask_.data();
	}
	return *this;
}

PipelineBuilder& PipelineBuilder::SetColorBlendInfo(const std::vector<VkPipelineColorBlendAttachmentState>& attachments, const VkPipelineColorBlendStateCreateInfo& info) {
	attachments_ = attachments;
	color_blend_info_ = info;
	color_blend_info_.attachmentCount = static_cast<uint32_t>(attachments_.size());
	color_blend_info_.pAttachments = attachments_.data();
	return *this;
}

PipelineBuilder& PipelineBuilder::SetPipelineLayout(VkPipelineLayout layout) {
	pipeline_layout_ = layout;
	return *this;
}

PipelineBuilder& PipelineBuilder::SetRenderPass(VkRenderPass renderPass){
	render_pass_ = renderPass;
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
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

	VkPipeline graphicsPipeline;
	if (vkCreateGraphicsPipelines(GetDevice(device_), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
		LogErrorDetaill("[Vulkan][Pipeline]: Faile to Create Pileline");
	}
	return graphicsPipeline;
}

} //namespace Driver::Vulkan