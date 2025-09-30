#include "vulkan_context.h"
#include "core/events/event_system.h"
#include <atomic>
#include <cstdint>

namespace Context {
using namespace Driver::Vulkan;

VulkanContext::VulkanContext(const Platform::AppWindow& window) :
		window_(window) {
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

	CommandBuilder builder{ *commands_[current_frame_] };
	builder.BeginRenderPass(*renderpass_, swapchain_framebuffer_->Get(index), extent)
			.BindGraphicsPipeline(*pipeline_)
			.SetViewport(extent)
			.SetScissor(extent)
			.Draw(3, 1, 0, 0)
			.EndRenderPass();
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
	vkDeviceWaitIdle(*vkdevice_);
}

bool VulkanContext::ReCreazteSwapChain() {
	using namespace Context::Renderer::Event;

	if (renderer_paused_.load(std::memory_order_acquire)) {
		return false;
	}

	auto wh = window_.GetWindowSizeInPixel();
	if (wh.first == 0 || wh.second == 0) {
		LogWarringDetail("Window minimized during swapchain recreation, aborting");
		return false;
	}

	vkDeviceWaitIdle(*vkdevice_);

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

	return true;
	//Skip NextFrame
	// auto& dispatch = Core::Event::EventSystem::Instance().GetRendererDispatcher();
	// dispatch.publish<RenderNextFrameEvent>({.next_count_ = 1});
}

} //namespace Context