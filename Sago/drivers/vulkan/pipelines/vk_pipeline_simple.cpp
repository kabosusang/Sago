#include "vk_pipeline_simple.h"
#include "vk_pipeline_build.h"

namespace Driver::Vulkan {

VulkanSimplePipeline::VulkanSimplePipeline(const VulkanDevice& device, const VulkanSwapchain& swapchain,const VulkanSimpleRenderPass& renderpass) :
		device_(device), swapchain_(swapchain),renderpass_(renderpass) {
	CreatePipeline();
}

VulkanSimplePipeline::~VulkanSimplePipeline() noexcept {
	const auto& device = GetDevice(device_);
	shader_manager_.Release(device);
	if (pipeline_) {
		vkDestroyPipeline(device, pipeline_, nullptr);
	}
	if (pipeline_layout_) {
		vkDestroyPipelineLayout(device, pipeline_layout_, nullptr);
	}
}

void VulkanSimplePipeline::CreatePipelineImpl() {
	//auto module = shader_manager_.LoadShaderModuleAsync(GetDevice(device_),"triangle_vert.spv","triangle_frag.spv");
	//auto [vert,frag] = module.GetValue();

	auto vert = shader_manager_.LoadShader(device_, "triangle_vert.spv");
	auto frag = shader_manager_.LoadShader(device_, "triangle_frag.spv");

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0; // Optional
	pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

	if (vkCreatePipelineLayout(device_, &pipelineLayoutInfo, nullptr, &pipeline_layout_) != VK_SUCCESS) {
		LogErrorDetail("[Vulkan][Pipeline]: Faile to Create Pileline Layout ");
	}

	PipelineBuilder builder(device_);
	auto vertShader = ShaderModuleInfo<ShaderStrategy::Vert>{ vert };
	auto fragShader = ShaderModuleInfo<ShaderStrategy::Frag>{ frag };

	builder.SetShaderModule(vertShader, fragShader)
			.SetPipelineLayout(pipeline_layout_);
	SetDefaultStatus(builder);
	builder.SetRenderPass(renderpass_.GetRenderPass());
	pipeline_ = builder.Build();
}

void VulkanSimplePipeline::SetDefaultStatus(PipelineBuilder& builder) const {
	//Vertex Input

	//AssemblyState
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;
	builder.SetInputAssemblyInfo(inputAssembly);

	//Dynamic
	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	//ViewPort
	auto& extent = swapchain_.GetExtent();
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)extent.width;
	viewport.height = (float)extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	//Scissor
	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = extent;

	//Rasterizataion
	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	//rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	//lineWidth > 1.0f Need wideLines(GPU)
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE; //Shadow
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

	//Multisample
	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	//multisampling.pSampleMask = nullptr; // Optional todo pointer
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional

	//Attachment
	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

	//Color Blend
	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	//colorBlending.attachmentCount = 1;
	//colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	builder.SetVertexInputInfo()
			.SetInputAssemblyInfo(inputAssembly)
			.SetDynamicStateInfo(dynamicStates)
			.SetViewports({viewport})
			.SetScissors({scissor})
			.SetViewportStateInfo()
			.SetRasterizationInfo(rasterizer)
			.SetMultisampleInfo({},multisampling)
			.SetColorBlendInfo({colorBlendAttachment}, colorBlending);
}


} //namespace Driver::Vulkan