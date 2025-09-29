#ifndef SG_RENDERER_CONTEXT_H
#define SG_RENDERER_CONTEXT_H

#include <array>
#include <atomic>
#include <functional>
#include <memory>

#include "context/context_base.h"
#include "context/event/renderer_event.h"
#include "meta/traits/class_traits.h"

//Memory
#include "core/memory/buffer/ring_buffer.h"
#include "core/memory/lockfree/SPSC/array.h"

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
	using Event = Event::RendererEventType;
	using EventQueue = Core::Memory::LockFreeArray<Event>;
	//using RingBuffer = Core::Memory::RingBuffer<typename T, size_t Capacity>

	RendererContext(const Platform::AppWindow&, const Controller::FrameRateController&);
	RendererContext(const RendererContext&) = delete;
	RendererContext(const RendererContext&&) = delete;
	RendererContext& operator=(const RendererContext&) = delete;
	RendererContext& operator=(const RendererContext&&) = delete;
	~RendererContext() noexcept;

	void PutEvent(callable_t&& callable) noexcept;
	inline void PutEvent(Event event) {
		event_queue_.push(event);
		{
			std::lock_guard lock(mutex_);
			work_pending_ = true;
		}
		work_cv_.notify_one();
	}

	void RequestFrame() noexcept;
	//CRPT
private:
	void InitImpl();
	void ListenEventImpl();

private:
	void Tick() noexcept;
	void HandleEvent(const Event&);
	void ProcessTasks(int buffer_index) noexcept;

private:
	const Platform::AppWindow& window_;
	const Controller::FrameRateController& fpscontroller_;
	std::atomic<bool> running_{ true };
private:
	std::jthread thread_;
	std::mutex mutex_;
	std::condition_variable work_cv_;
	bool work_pending_ = false;
	//Buffer
	std::array<std::vector<callable_t>, 2> task_buffers_;
	std::atomic<int> write_index_{ 0 };
	//Event Type
	EventQueue event_queue_;
	//Ring Buff
private:
	std::unique_ptr<VulkanContext> vk_context_;
};

} //namespace Context::Renderer

#endif