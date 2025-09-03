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
	
	command_ = std::make_unique<Command>(vkinitail_->GetPhysicalDevice(),vkdevice_->GetDevice(),vkdevice_->GetGraphyciQueue());

	//Async
	image_available_semaphore_ = std::make_unique<Semaphore>(vkdevice_->GetDevice());
	render_finished_semaphore_ = std::make_unique<Semaphore>(vkdevice_->GetDevice());

	inflight_fence_ = std::make_unique<Fence>(vkdevice_->GetDevice(),VK_FENCE_CREATE_SIGNALED_BIT);
}

void VulkanContext::Renderer(){
	//LogInfo("Tick Renderer");
	//WaitForPreviousFrame();
	//auto index = GetImageForSwapChain();
	//Command
	//RendererCommand(index);
	

	
}

void VulkanContext::WaitForPreviousFrame() const {
	inflight_fence_->wait();
	inflight_fence_->reset();
}

uint32_t VulkanContext::GetImageForSwapChain() const{
	uint32_t imageIndex{};
	vkAcquireNextImageKHR(*vkdevice_,*vkswapchain_,UINT64_MAX,*image_available_semaphore_,VK_NULL_HANDLE,&imageIndex);
	return imageIndex;
}

void VulkanContext::RendererCommand(uint32_t) const{
	command_->Reset();
	command_->BeginRecording();



}





VulkanContext::~VulkanContext() {
	
}

} //namespace Context