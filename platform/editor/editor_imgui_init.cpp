#include "editor_imgui_init.h"
#include <memory>
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

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	float scale = 1.50f;
	ImGuiStyle& style = ImGui::GetStyle();

	ImGui::StyleColorsDark();

	//初始化 SDL 后端
	if (!ImGui_ImplSDL3_InitForVulkan(window)) {
		LogError("[Platform][EditorUI]: ❌ Failed to initialize ImGui SDL backend");
		return;
	}

	using namespace Driver::Vulkan;
	//Vk DescriptorPoolSize
	despool_ = std::make_unique<VulkanDescriptorPoolNoSet<128>>(
			data_->device);
	//Renderpass
	renderpass_ = std::make_unique<VulkanUIRenderPass>(
			data_->device,
			data_->format);
	// 创建framebuffer
	frambuffer_ = std::make_unique<VulkanFrameBuffer>(VulkanFrameBuffer::CreateInfo{
			.device = data_->device,
			.renderPass = renderpass_->GetRenderPass(),
			.attachments = data_->imageview,
			.width = data_->extent.width,
			.height = data_->extent.height,
			.layers = 1 });
	//5. 配置 Vulkan 初始化信息
	ImGui_ImplVulkan_InitInfo init_info{};
	init_info.ApiVersion = VK_API_VERSION_1_4;
	init_info.Instance = data_->instance;
	init_info.PhysicalDevice = data_->physical_device;
	init_info.Device = data_->device;
	init_info.QueueFamily = data_->queuefamily;
	init_info.Queue = data_->queue;
	init_info.PipelineCache = nullptr;
	init_info.DescriptorPool = despool_->GetPool();
	init_info.MinImageCount = 3;
	init_info.ImageCount = 3;
	//init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	init_info.Allocator = nullptr;
	init_info.CheckVkResultFn = nullptr;

	init_info.PipelineInfoMain.RenderPass = renderpass_->GetRenderPass();
	init_info.PipelineInfoMain.Subpass = 0;
	init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

	bool is_success = ImGui_ImplVulkan_Init(&init_info);
	if (!is_success) {
		LogErrorDetail("[Platform][EditorUI]: ❌ Failed to ImGui_ImplVulkan_Init");
	}

	const float k_base_font_size = 14.0f;
	initialized_ = true;
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
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	data_.reset();
	initialized_ = false;
	LogInfo("[EditorUI]: Shutdown completed");
}

} // namespace Platform