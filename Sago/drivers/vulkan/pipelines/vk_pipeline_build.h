#ifndef SG_VULKAN_PIPELINE_BUILD_H
#define SG_VULKAN_PIPELINE_BUILD_H
#include "drivers/vulkan/extensions/vk_swapchain.h"
#include "drivers/vulkan/vk_device.h"
#include <concepts>
#include <string_view>
#include <vector>

namespace Driver::Vulkan {

struct ShaderStrategy {
	struct Vert {
		static constexpr VkShaderStageFlagBits stage = VK_SHADER_STAGE_VERTEX_BIT;
	};
	struct Frag {
		static constexpr VkShaderStageFlagBits stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	};
	struct Geom {
		static constexpr VkShaderStageFlagBits stage = VK_SHADER_STAGE_GEOMETRY_BIT;
	};
	struct Comp {
		static constexpr VkShaderStageFlagBits stage = VK_SHADER_STAGE_COMPUTE_BIT;
	};
};

template <typename T>
concept ShaderStrategyType =
		std::is_same_v<T, ShaderStrategy::Vert> ||
		std::is_same_v<T, ShaderStrategy::Frag> ||
		std::is_same_v<T, ShaderStrategy::Geom> ||
		std::is_same_v<T, ShaderStrategy::Comp>;

template <ShaderStrategyType Strategy>
struct ShaderModuleInfo {
	VkShaderModule module_;
	static constexpr Strategy strategy_{};
};

static constexpr const char* EngineShaderEntry = "main";

class PipelineBuilder {
public:
	PipelineBuilder(const VulkanDevice& device);

	PipelineBuilder(const PipelineBuilder&) = delete;
	PipelineBuilder(PipelineBuilder&&) = delete;
	PipelineBuilder& operator=(const PipelineBuilder&) = delete;
	PipelineBuilder& operator=(const PipelineBuilder&&) = delete;

private:
	template <ShaderStrategyType Strategy>
	VkPipelineShaderStageCreateInfo CreateShaderStage(VkShaderModule module, std::string_view entry = { "main" }) const {
		VkPipelineShaderStageCreateInfo stageInfo{};
		stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stageInfo.stage = Strategy::stage;
		stageInfo.module = module;
		stageInfo.pName = entry.data();
		return stageInfo;
	}

public:
	template <typename... Strategies>
	PipelineBuilder& SetShaderModule(const ShaderModuleInfo<Strategies>&... modules) {
		shader_stages_.clear();
		(shader_stages_.push_back(CreateShaderStage<Strategies>(modules.module_, EngineShaderEntry)), ...);
		return *this;
	}

	///////////////////VertexInPut//////////////////////////
	PipelineBuilder& SetVertexInputInfo();
	PipelineBuilder& SetVertexInputInfo(const std::vector<VkVertexInputBindingDescription>& binds, const std::vector<VkVertexInputAttributeDescription>& attributes);
	PipelineBuilder& SetInputAssemblyInfo(const VkPipelineInputAssemblyStateCreateInfo& info);
	///////////////////ViewPort////////////////////////////
	PipelineBuilder& SetDynamicStateInfo(const std::vector<VkDynamicState>& dynamic);
	PipelineBuilder& SetDynamicStateInfo(std::vector<VkDynamicState>&& dynamic);
	PipelineBuilder& SetViewports(const std::vector<VkViewport>& viewports);
	PipelineBuilder& SetViewports(std::vector<VkViewport>&& viewports);
	PipelineBuilder& SetScissors(const std::vector<VkRect2D>& scissors);
	PipelineBuilder& SetScissors(std::vector<VkRect2D>&& scissors);
	PipelineBuilder& SetViewportStateInfo();
	PipelineBuilder& SetViewportStateInfo(const VkPipelineViewportStateCreateInfo& info);
	///////////////////////////////////////////////////////
	PipelineBuilder& SetRasterizationInfo(const VkPipelineRasterizationStateCreateInfo& info);
	PipelineBuilder& SetMultisampleInfo(const std::vector<VkSampleMask>& masks, const VkPipelineMultisampleStateCreateInfo& info);
	PipelineBuilder& SetColorBlendInfo(const std::vector<VkPipelineColorBlendAttachmentState>& attachments, const VkPipelineColorBlendStateCreateInfo& info);

	PipelineBuilder& SetPipelineLayout(VkPipelineLayout layout);
	PipelineBuilder& SetRenderPass(VkRenderPass renderPass);

	VkPipeline Build();

private:
	const VulkanDevice& device_;

private:
	std::vector<VkPipelineShaderStageCreateInfo> shader_stages_;
	std::vector<VkDynamicState> dynamics_;
	std::vector<VkViewport> viewports_;
	std::vector<VkRect2D> scissors_;
	std::vector<VkSampleMask> samplemask_;
	std::vector<VkPipelineColorBlendAttachmentState> attachments_;

private:
	VkPipelineVertexInputStateCreateInfo vertex_input_info_{};
	VkPipelineInputAssemblyStateCreateInfo input_assembly_info_{};
	VkPipelineDynamicStateCreateInfo dynamic_state_info_{};
	///////////////////////////View Port////////////////////////////////////
	VkPipelineViewportStateCreateInfo viewport_state_info_{};
	//////////////////////////////////////////////////////////////////////////
	VkPipelineRasterizationStateCreateInfo rasterization_info_{};
	VkPipelineMultisampleStateCreateInfo multisample_info_{};
	VkPipelineColorBlendStateCreateInfo color_blend_info_{};
	VkPipelineLayout pipeline_layout_ = VK_NULL_HANDLE;
	VkRenderPass render_pass_ = VK_NULL_HANDLE;
};

} //namespace Driver::Vulkan

#endif