#ifndef SG_VULKAN_CONTEXT_H
#define SG_VULKAN_CONTEXT_H

//platform
#include "window_interface.h"
//vulkan
#include "drivers/vulkan/vk_instance.h"
#include "drivers/vulkan/vk_device.h"
#include "drivers/vulkan/extensions/vk_surface.h"
#include "drivers/vulkan/extensions/vk_swapchain.h"
#include "drivers/vulkan/renderpass/vk_renderpass_simple.h"
#include "drivers/vulkan/renderpass/vk_frambuffer.h"
#include "drivers/vulkan/pipelines/vk_pipeline_simple.h"
#include "drivers/vulkan/commands/vk_commandbuild.h"
#include "drivers/vulkan/commands/vk_semaphore.h"
#include "drivers/vulkan/commands/vk_fence.h"
#include <cstdint>


namespace Context {

class VulkanContext {
public:
	void Renderer();

public:
	using FrameBuffer = Driver::Vulkan::VulkanFrameBuffer;
	using RenderPass = Driver::Vulkan::VulkanSimpleRenderPass;
	using Pipeline = Driver::Vulkan::VulkanSimplePipeline;

	VulkanContext(const Platform::AppWindow&);
	~VulkanContext();
	VulkanContext(const VulkanContext&) = delete;
	VulkanContext& operator=(const VulkanContext&) = delete;
	VulkanContext(VulkanContext&&) = delete;
	VulkanContext& operator=(VulkanContext&&) = delete;

	auto& GetDevice() { return *vkdevice_; }
	auto& GetSwapChain(){return *vkswapchain_;}

private:
    const Platform::AppWindow& window_;
	std::unique_ptr<Driver::Vulkan::VulkanInitializer> vkinitail_;
	std::unique_ptr<Driver::Vulkan::VulkanDevice> vkdevice_;
	std::unique_ptr<Driver::Vulkan::VulkanSurface> vksurface_;
	std::unique_ptr<Driver::Vulkan::VulkanSwapchain> vkswapchain_;

	//RenderPass
	std::unique_ptr<RenderPass> renderpass_;
	//Pipeline 
	std::unique_ptr<Pipeline> pipeline_;
	//FrameBuffer
	std::unique_ptr<FrameBuffer> swapchain_framebuffer_;
private:
	using Command = Driver::Vulkan::VulkanCommand;
	//Command
	std::unique_ptr<Command> command_;
private:
	using Semaphore = Driver::Vulkan::VulkanSemaphore;
	using Fence = Driver::Vulkan::VulkanFence;
	//sync
	std::unique_ptr<Semaphore> image_available_semaphore_;
	std::unique_ptr<Semaphore> render_finished_semaphore_;
	std::unique_ptr<Fence> inflight_fence_;
private:
	void WaitForPreviousFrame() const;
	uint32_t GetImageForSwapChain() const;
	void RendererCommand(uint32_t) const;
	void Submit()const;
	void Present(uint32_t)const;
};

} //namespace Context

#endif