#ifndef SG_VULKAN_CONTEXT_H
#define SG_VULKAN_CONTEXT_H
//#include <expected>
//platform
#include "window_interface.h"
//vulkan
#include "drivers/vulkan/commands/vk_commandbuild.h"
#include "drivers/vulkan/commands/vk_commandpool.h"
#include "drivers/vulkan/commands/vk_fence.h"
#include "drivers/vulkan/commands/vk_semaphore.h"
#include "drivers/vulkan/extensions/vk_surface.h"
#include "drivers/vulkan/extensions/vk_swapchain.h"
#include "drivers/vulkan/pipelines/vk_pipeline_simple.h"
#include "drivers/vulkan/renderpass/vk_frambuffer.h"
#include "drivers/vulkan/renderpass/vk_renderpass_simple.h"
#include "drivers/vulkan/vk_device.h"
#include "drivers/vulkan/vk_instance.h"
//memory
#include "drivers/vulkan/memory/vk_vma_allocator.h"
#include "drivers/vulkan/commands/vk_upload_manager.h"


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
	auto& GetSwapChain() { return *vkswapchain_; }

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

public:
	bool frame_buffer_resized_{ false };
	std::atomic<bool> renderer_paused_{ false };

private:
	uint32_t current_frame_{};
	uint32_t max_frame_flight_{};
	//Asynch
	using Command = Driver::Vulkan::VulkanCommand;
	using Pool = Driver::Vulkan::VulkanCommandPool;
	//Command
	std::unique_ptr<Pool> commandpool_;
	std::vector<std::unique_ptr<Command>> commands_;

private:
	using Semaphore = Driver::Vulkan::VulkanSemaphore;
	using Fence = Driver::Vulkan::VulkanFence;
	std::vector<std::unique_ptr<Semaphore>> image_available_semaphores_;
	std::vector<std::unique_ptr<Semaphore>> render_finished_semaphores_;
	std::vector<std::unique_ptr<Fence>> inflight_fences_;
private:


private:
	bool ReCreazteSwapChain();
	//std::expected<uint32_t, bool>GetImageForSwapChain_();
	std::pair<uint32_t, bool> GetImageForSwapChain();
	void Present(uint32_t);
	void WaitForPreviousFrame() const;
	void ResetForFence() const;
	void RendererCommand(uint32_t) const;
	void Submit() const;

private:
	using VmaAlloactor = Driver::Vulkan::Memory::VulkanAllocator;
	using Buffer = Driver::Vulkan::Memory::VulkanAllocator::Buffer;
	using UploadManager = Driver::Vulkan::VulkanUploadManager;
	//Memory
	std::unique_ptr<VmaAlloactor> vma_allocator_;
	std::unique_ptr<UploadManager> upload_manager_;
	void CreateMemeoryAllocate();
	Buffer vertex_buffer_;
	Buffer index_buffer_;

};

} //namespace Context

#endif