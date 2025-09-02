#include "vulkan_context.h"

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
}

void VulkanContext::Renderer(){
	//LogInfo("Tick");

	
}

VulkanContext::~VulkanContext() {
	
}

} //namespace Context