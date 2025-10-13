#include "vulkan_context.h"
#include "core/events/event_system.h"

#include <cstdint>
#include <memory>
#include <sstream>

#include "editor/editor_imgui_init.h"

namespace Context {
using namespace Driver::Vulkan;
template<typename T>
std::string VulkanHandleToString(T handle) {
    std::stringstream ss;
    ss << reinterpret_cast<uintptr_t>(handle);
    return ss.str();
}

VulkanContext::VulkanContext(const Platform::AppWindow& window,
Platform::EditorUI& editor) :
		window_(window),editor_(editor) {
	vkinitail_ = std::make_unique<VulkanInitializer>();
	vksurface_ = std::make_unique<VulkanSurface>(window_, *vkinitail_);
	vkdevice_ = std::make_unique<VulkanDevice>(*vkinitail_, *vksurface_);
	vkswapchain_ = std::make_unique<VulkanSwapchain>(window_, *vkinitail_, *vksurface_, *vkdevice_);

	renderpass_ = std::make_unique<RenderPass>(GetDevice(), GetSwapChain());
	pipeline_ = std::make_unique<Pipeline>(GetDevice(), GetSwapChain(), *renderpass_);

	swapchain_framebuffer_ = std::make_unique<FrameBuffer>(VulkanFrameBuffer::CreateInfo{
			.device = GetDevice().GetDevice(),
			.renderPass = renderpass_->GetRenderPass(),
			.attachments = GetSwapChain().GetImageViews(),
			.width = GetSwapChain().GetExtent().width,
			.height = GetSwapChain().GetExtent().height,
	});

	//Asynch
	max_frame_flight_ = vkswapchain_->GetImages().size();
	auto indice_graphy = FindIndice(Graphy{}, vkinitail_->GetPhysicalDevice());
	commandpool_ = std::make_unique<VulkanCommandPool>(
			vkdevice_->GetDevice(),
			indice_graphy.family_.value());

	commands_.resize(max_frame_flight_);
	image_available_semaphores_.resize(max_frame_flight_);
	render_finished_semaphores_.resize(max_frame_flight_);
	inflight_fences_.resize(max_frame_flight_);
	for (int i = 0; i < max_frame_flight_; ++i) {
		image_available_semaphores_[i] = std::make_unique<Semaphore>(vkdevice_->GetDevice());
		render_finished_semaphores_[i] = std::make_unique<Semaphore>(vkdevice_->GetDevice());
		commands_[i] = commandpool_->CreateCommand(vkdevice_->GetGraphyciQueue());
		inflight_fences_[i] = std::make_unique<Fence>(vkdevice_->GetDevice(), VK_FENCE_CREATE_SIGNALED_BIT);
	}
	//images_in_flight_.resize(max_frame_flight,VK_NULL_HANDLE);
	CreateMemeoryAllocate();

	//PushEvent VulkanContext Data
	auto& dispatch = Core::Event::EventSystem::Instance().GetMainDispatcher();
	auto data = std::make_shared<VulkanContextData>(
			vkdevice_->GetDevice(),
			vkinitail_->GetInstance(),
			vkinitail_->GetPhysicalDevice(),
			vkdevice_->GetGraphyciQueue(),
			vkswapchain_->GetFormat(),
			vkswapchain_->GetExtent(),
			indice_graphy.family_.value(),
			vkswapchain_->GetImageViews()
		);
	using namespace Core::Event;
	dispatch.publish<RendererDataInitEvent>(
			{ data });
}

void VulkanContext::Renderer() {
	WaitForPreviousFrame();
	auto [index, result] = GetImageForSwapChain();
	if (!result) {
		return;
	}
	ResetForFence();
	//Command
	RendererCommand(index);
	Submit();
	Present(index);
	current_frame_ = (current_frame_ + 1) % max_frame_flight_;
}

void VulkanContext::WaitForPreviousFrame() const {
	inflight_fences_[current_frame_]->wait();
}

void VulkanContext::ResetForFence() const {
	inflight_fences_[current_frame_]->reset();
}

//Memory
#include "drivers/vulkan/memory/vertex.h"
using namespace Driver::Vulkan::Memory;
const std::vector<Vertex> vertices = {
	{ { -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
	{ { 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } },
	{ { 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } },
	{ { -0.5f, 0.5f }, { 1.0f, 1.0f, 1.0f } }
};

const std::vector<uint16_t> indices = {
	0, 1, 2, 2, 3, 0
};

std::pair<uint32_t, bool> VulkanContext::GetImageForSwapChain() {
	uint32_t imageIndex{};
	auto result = vkAcquireNextImageKHR(*vkdevice_, *vkswapchain_, UINT64_MAX, *image_available_semaphores_[current_frame_], VK_NULL_HANDLE, &imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		return { 0, ReCreazteSwapChain() };
	} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) [[unlikely]] {
		LogErrorDetail("[Context][VulkanContext] Failed To AcquireNextImage");
	}
	return { imageIndex, true };
}

void VulkanContext::RendererCommand(uint32_t index) const {
	using namespace Driver::Vulkan;
	commands_[current_frame_]->Reset();
	commands_[current_frame_]->BeginRecording();
	const auto& extent = vkswapchain_->GetExtent();
	//renderer_pass
	CommandBuilder builder{ *commands_[current_frame_] };
	VkBuffer vertexBuffer = vertex_buffer_.buffer;
	VkDeviceSize offsets[] = { 0 };
	VkBuffer indexBuffer = index_buffer_.buffer;
	builder.BeginRenderPass(*renderpass_, swapchain_framebuffer_->Get(index), extent)
			.BindGraphicsPipeline(*pipeline_)
			.BindVertexBuffers(0, { vertexBuffer }, { 0 })
			.BindIndexBuffer(indexBuffer, 0, VK_INDEX_TYPE_UINT16)
			.SetViewport(extent)
			.SetScissor(extent)
			.DrawIndexed(indices.size())
			//.Draw(3, 1, 0, 0)
			.EndRenderPass();
	//ui_pass
	editor_.RecordRenderCommands(commands_[current_frame_]->getHandle(), index);
	commands_[current_frame_]->EndRecording();
	
}

void VulkanContext::Submit() const {
	commands_[current_frame_]->Submit({ *image_available_semaphores_[current_frame_] },
			{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT },
			{ *render_finished_semaphores_[current_frame_] },
			*inflight_fences_[current_frame_]);
}

void VulkanContext::Present(uint32_t imageindex) {
	auto&& [result, str] = commands_[current_frame_]->Present(
			vkdevice_->GetPresentQueue(),
			{ *render_finished_semaphores_[current_frame_] },
			*vkswapchain_,
			imageindex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || frame_buffer_resized_) {
		if (!ReCreazteSwapChain()) {
			frame_buffer_resized_ = true;
			LogInfoDetail("Swapchain recreation deferred");
		} else {
			frame_buffer_resized_ = false;
		}
	} else if (result != VK_SUCCESS) {
		LogErrorDetail("[Vulkan][Present] Failed To Present SwapChain Image");
	}
}

VulkanContext::~VulkanContext() {
	WaitForDeviceIdle();

	if (vma_allocator_) {
		vma_allocator_->DestroyBuffer(vertex_buffer_);
		vma_allocator_->DestroyBuffer(index_buffer_);
	}
}

bool VulkanContext::ReCreazteSwapChain() {
	using namespace Context::Renderer::Event;

	auto wh = window_.GetWindowSizeInPixel();
	if (wh.first == 0 || wh.second == 0) {
		LogWarringDetail("Window minimized during swapchain recreation, aborting");
		return false;
	}

	WaitForDeviceIdle();

	swapchain_framebuffer_.reset();
	vkswapchain_->RecreateSwapchain();
	
	VkExtent2D newExtent = vkswapchain_->GetExtent();
	if (newExtent.width == 0 || newExtent.height == 0) {
		LogWarringDetail("Failed to create valid swapchain extent");
		return false;
	}

	swapchain_framebuffer_ = std::make_unique<FrameBuffer>(VulkanFrameBuffer::CreateInfo{
			.device = GetDevice().GetDevice(),
			.renderPass = renderpass_->GetRenderPass(),
			.attachments = GetSwapChain().GetImageViews(),
			.width = GetSwapChain().GetExtent().width,
			.height = GetSwapChain().GetExtent().height,
	});

	editor_.RecreateFrameBuffer(GetSwapChain().GetImageViews(),newExtent);

	return true;

	// auto& dispatch = Core::Event::EventSystem::Instance().GetRendererDispatcher();
	// dispatch.publish<RenderNextFrameEvent>({.next_count_ = 1});
}

void VulkanContext::CreateMemeoryAllocate() {
	using VulkanAllocator = Memory::VulkanAllocator;

	Memory::AllocatorConfig config{};
	config.enable_bufferdevice_address = false;

	vma_allocator_ = std::make_unique<VulkanAllocator>(
			vkinitail_->GetInstance(),
			vkdevice_->GetDevice(),
			vkinitail_->GetPhysicalDevice(),
			config);

	auto indice_graphy = FindIndice(Graphy{}, vkinitail_->GetPhysicalDevice());

	upload_manager_ = std::make_unique<UploadManager>(*vkdevice_, indice_graphy.family_.value());

	//vertex
	vertex_buffer_ = vma_allocator_->CreateVertexBuffer<Vertex>(
			vertices.size(),
			"TriangleVertices");

	auto vertexStaging = vma_allocator_->CreateStagingBuffer(vertices, "VertexStaging");
	upload_manager_->UploadBufferData(*vma_allocator_, vertexStaging, vertex_buffer_);

	//index
	index_buffer_ = vma_allocator_->CreateIndexBuffer<uint16_t>(indices.size());
	auto indexStaging = vma_allocator_->CreateStagingBuffer(indices, "IndexStaging");
	upload_manager_->UploadBufferData(*vma_allocator_, indexStaging, index_buffer_);

	vma_allocator_->DestroyBuffer(vertexStaging);
	vma_allocator_->DestroyBuffer(indexStaging);
}

} //namespace Context