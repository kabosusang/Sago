#include "vulkan_context.h"

#include <cstdint>

namespace Context {

VulkanContext::VulkanContext(const Platform::AppWindow& window) :
		window_(window) {
	using namespace Driver::Vulkan;
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
	max_frame_flight = vkswapchain_->GetImages().size();
	auto indice_graphy = FindIndice(Graphy{}, vkinitail_->GetPhysicalDevice());
	commandpool_ = std::make_unique<VulkanCommandPool>(
			vkdevice_->GetDevice(),
			indice_graphy.family_.value());

	commands_.resize(max_frame_flight);
	image_available_semaphores_.resize(max_frame_flight);
	render_finished_semaphores_.resize(max_frame_flight);
	inflight_fences_.resize(max_frame_flight);
	for (int i = 0; i < max_frame_flight; ++i) {
		image_available_semaphores_[i] = std::make_unique<Semaphore>(vkdevice_->GetDevice());
		render_finished_semaphores_[i] = std::make_unique<Semaphore>(vkdevice_->GetDevice());
		commands_[i] = commandpool_->CreateCommand(vkdevice_->GetGraphyciQueue());
		inflight_fences_[i] = std::make_unique<Fence>(vkdevice_->GetDevice(), VK_FENCE_CREATE_SIGNALED_BIT);
	}
	//images_in_flight_.resize(max_frame_flight,VK_NULL_HANDLE);
}

void VulkanContext::Renderer() {
	//LogInfo("Tick Renderer");
	WaitForPreviousFrame();
	auto index = GetImageForSwapChain();
	//Command
	RendererCommand(index);
	Submit();
	Present(index);
	current_frame_ = (current_frame_ + 1) % max_frame_flight;
}

void VulkanContext::WaitForPreviousFrame() const {
	inflight_fences_[current_frame_]->wait();
	inflight_fences_[current_frame_]->reset();
}

uint32_t VulkanContext::GetImageForSwapChain() const{
	uint32_t imageIndex{};
	vkAcquireNextImageKHR(*vkdevice_, *vkswapchain_, UINT64_MAX, *image_available_semaphores_[current_frame_], VK_NULL_HANDLE, &imageIndex);
	// if (images_in_flight_[imageIndex] != VK_NULL_HANDLE) {
	// 	vkWaitForFences(*vkdevice_, 1, &images_in_flight_[imageIndex], VK_TRUE, UINT64_MAX);
	// }
	// images_in_flight_[imageIndex] = inflight_fences_[current_frame_]->getHandle();
	return imageIndex;
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

void VulkanContext::Present(uint32_t imageindex) const {
	commands_[current_frame_]->Present(
			vkdevice_->GetPresentQueue(),
			{ *render_finished_semaphores_[current_frame_] },
			*vkswapchain_,
			imageindex);
}

VulkanContext::~VulkanContext() {
	vkDeviceWaitIdle(*vkdevice_);
}

} //namespace Context