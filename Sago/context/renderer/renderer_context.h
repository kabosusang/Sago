#ifndef SG_RENDERER_CONTEXT_H
#define SG_RENDERER_CONTEXT_H

#include <functional>
#include <memory>

#include "context/context_base.h"
#include "meta/traits/class_traits.h"
#include "context/event/renderer_event.h"
//Memory
#include "core/memory/lockfree/queue.h"
#include "core/memory/buffer/ring_buffer.h"
//Controller
#include "context/controller/framerate_controller.h"
//platform
#include "window/window_sdl.h"
#include "window_interface.h"
//Vulkan
#include "vulkan_context.h"

//Pipeline
#include "drivers/vulkan/renderpass/vk_renderpass_simple.h"
#include "drivers/vulkan/pipelines/vk_pipeline_simple.h"


namespace Context::Renderer {

class RendererContext : public Context::ContextBase<RendererContext> {
	friend class ContextBase<RendererContext>;
	DEFINE_CLASS_NAME(RendererContext);
public:
	using callable_t = std::function<void()>;
	using Event     = 	Event::RendererEventType;
	using EventQueue = Core::Memory::LockFreeQueue<Event>;
	//using RingBuffer = Core::Memory::RingBuffer<typename T, size_t Capacity>
	using RenderPass = Driver::Vulkan::VulkanSimpleRenderPass;
	using Pipeline = Driver::Vulkan::VulkanSimplePipeline;


	RendererContext(const Platform::AppWindow&,const Controller::FrameRateController&);
	RendererContext(const RendererContext&) = delete;
	RendererContext(const RendererContext&&) = delete;
	RendererContext& operator=(const RendererContext&) = delete;
	RendererContext& operator=(const RendererContext&&) = delete;
	~RendererContext() noexcept;

	void PutEvent(callable_t&& callable) noexcept;
	inline void PutEvent(Event event){
		queue_.push(event);
	}
private:
	void InitImpl();
	void Tick() noexcept;
	void HandleEvent(const Event&);
private:
	const Platform::AppWindow& window_;
	const Controller::FrameRateController& fpscontroller_;

private:
	std::jthread thread_;
	//Event Loop
	std::vector<callable_t> writebuffer_;
	std::mutex mutex_;
	std::condition_variable cv_;
	bool running_{ true };

	//Event Type
	EventQueue queue_;

	//Ring Buff
private:
	std::unique_ptr<VulkanContext> vk_context_;
private:
	//RenderPass
	std::unique_ptr<RenderPass> renderpass_;
	//Pipeline 
	std::unique_ptr<Pipeline> pipeline_;
};

} //namespace Context::Renderer

#endif