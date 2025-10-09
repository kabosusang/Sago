#include "editor_imgui_init.h"
#include <utility>

#include "drivers/vulkan/vk_context_data.h"
#include "window/window_sdl.h"
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_vulkan.h>
#include <cstdint>
#include <sstream>

#include "drivers/vulkan/vk_log.h"

namespace Platform {

template <typename T>
std::string VulkanHandleToString(T handle) {
	std::stringstream ss;
	ss << reinterpret_cast<uintptr_t>(handle);
	return ss.str();
}

EditorUI::EditorUI(const AppWindow& window) :
		window_(window) {
}

EditorUI::~EditorUI() {
	Shutdown();
}

using namespace Driver::Vulkan;

void EditorUI::Init_Imgui(std::shared_ptr<VulkanContextData> data) {
	using namespace Core::Log;
	auto window = window_.GetRawImpl();
	data_ = std::move(data);

    IMGUI_CHECKVERSION();
	ImGui::CreateContext();


	// // 4. 初始化 SDL 后端
	// if (!ImGui_ImplSDL3_InitForVulkan(window)) {
	// 	LogError("[Platform][EditorUI]: ❌ Failed to initialize ImGui SDL backend");
	// 	return;
	// }
	// LogInfo("[Platform][EditorUI]: ✅ SDL backend initialized");

	// // 5. 配置 Vulkan 初始化信息
	// ImGui_ImplVulkan_InitInfo init_info = {};
	// init_info.Instance = data_->instance;
	// init_info.PhysicalDevice = data_->physical_device;
	// init_info.Device = data_->device;
	// init_info.Queue = data_->queue;
	// init_info.DescriptorPool = descriptor_pool_;
	// init_info.MinImageCount = 3;
	// init_info.ImageCount = 3;
	// init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	// init_info.UseDynamicRendering = true;

}

void EditorUI::NewFrame() {
	if (!initialized_) {
		return;
	}

	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();
}

void EditorUI::Render(VkCommandBuffer commandBuffer, VkImageView targetImageView) {
	if (!initialized_) {
		return;
	}

	auto [width, height] = window_.GetWindowSizeInPixel();

	// 设置动态渲染附件
	VkRenderingAttachmentInfo colorAttachment = {};
	colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	colorAttachment.imageView = targetImageView;
	colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD; // 加载现有内容（在场景渲染之后）
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // 保存绘制结果

	// 设置动态渲染信息
	VkRenderingInfo renderingInfo = {};
	renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
	renderingInfo.renderArea.offset = { 0, 0 };
	renderingInfo.renderArea.extent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
	renderingInfo.layerCount = 1;
	renderingInfo.colorAttachmentCount = 1;
	renderingInfo.pColorAttachments = &colorAttachment;

	// 开始动态渲染
	vkCmdBeginRendering(commandBuffer, &renderingInfo);

	// 渲染 ImGui
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

	// 结束动态渲染
	vkCmdEndRendering(commandBuffer);
}

void EditorUI::ProcessEvent(SDL_Event& event) {
	if (!initialized_) {
		return;
	}
	ImGui_ImplSDL3_ProcessEvent(&event);
}

void EditorUI::Shutdown() {
	if (!initialized_) {
		return;
	}

	if (device_ != VK_NULL_HANDLE) {
		vkDeviceWaitIdle(device_);
	}

	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();
	if (descriptor_pool_ != VK_NULL_HANDLE) {
		vkDestroyDescriptorPool(device_, descriptor_pool_, nullptr);
		descriptor_pool_ = VK_NULL_HANDLE;
	}

	initialized_ = false;
	LogInfo("[Platform][EditorUI]: ImGui shutdown completed");
}

} // namespace Platform