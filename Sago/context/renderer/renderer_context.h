#ifndef SG_RENDERER_CONTEXT_H
#define SG_RENDERER_CONTEXT_H

#include <functional>

#include "context/context_base.h"
#include "meta/traits/class_traits.h"
#include "core/memory/lockfree/queue.h"
#include "context/event/renderer_event.h"
//Controller
#include "context/controller/framerate_controller.h"
//platform
#include "window/window_sdl.h"
#include "window_interface.h"
//Vulkan
#include "vulkan_context.h"



namespace Context::Renderer {

class RendererContext : public Context::ContextBase<RendererContext> {
	friend class ContextBase<RendererContext>;
	DEFINE_CLASS_NAME(RendererContext);
public:
	using callable_t = std::function<void()>;
	using Event     = 	Event::RendererEventType;
	using EventQueue = Core::Memory::LockFreeQueue_Cas<Event>;

	RendererContext(const Platform::AppWindow&, const Controller::FrameRateController&);

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
	//Event Loop
	std::vector<callable_t> m_writeBuffer;
	std::mutex m_mutex;
	std::condition_variable m_condVar;
	bool m_running{ true };

	//Event Type
	EventQueue queue_;
private:
	std::jthread thread_;
	std::unique_ptr<VulkanContext> vk_context_;
};

} //namespace Context::Renderer

#endif