#include "engine_context.h"

#include <atomic>
#include <functional>
#include <mutex>

#include "core/events/event_system.h"
#include "event/renderer_event.h"
#include "window/window_sdl.h"

namespace Context {

using namespace Core::Event;
using namespace Context::Renderer::Event;
EngineContext::EngineContext() {
	using namespace Platform;

	window_ = std::make_unique<AppWindow>();
	if (!window_) {
		LogInfoDetail("Context Window Create Error");
	}
	Init();
	InitListenEvent();
}

void EngineContext::ListenEventImpl() {
	auto& dispatch = EventSystem::Instance().GetMainDispatcher();
	auto& dispatch_renderer = EventSystem::Instance().GetRendererDispatcher();

	dispatch.subscribe<KeyEvent>([](const KeyEvent& e) {
		LogInfoDetail("KeyDown{}", e.key_code_);
	});

	dispatch.subscribe<WindowMinimizeEvent>([&](const WindowMinimizeEvent& e) {
		if (e.minimized_) {
			this->is_paused_.store(true, std::memory_order_release);
			dispatch_renderer.publish(RendererPauseEvent{ .paused_ = true });
		} else {
			this->is_paused_.store(false, std::memory_order_release);
			dispatch_renderer.publish(RendererPauseEvent{ .paused_ = false });
		}
	});

	dispatch.subscribe<WindowResizeEvent>([&](const WindowResizeEvent& e) {
		if (e.width_ > 0 && e.height_ > 0) {
			dispatch_renderer.publish(SwapchainRecreateEvent{ .width_ = e.width_, .height_ = e.height_ });
		}
	});

	// renderer_->PutEvent(Event::RendererEventType::kRendererFrame);
	// renderer_->PutEvent(
	// 		[&]() {
	// 			LogInfo("Event_01 is running");
	// 		});
	// int i = 10;
	// renderer_->PutEvent(
	// 		[=]() {
	// 			LogInfo("Event_02 is running{}",i);
	// 		});
}

/**
 * @brief Init All Thread
 *
 */
void EngineContext::InitImpl() {
	using namespace Renderer;
	renderer_ = std::make_unique<RendererContext>(std::ref(*window_), std::ref(fps_controller_));
}

void EngineContext::Tick() {
	//EventProcess
	EventSystem::Instance().ProcessUpToEvents<ThreadCategory::Main>(256);
	if (is_paused_.load(std::memory_order_acquire)) {
		return;
	}
	//EventSystem::Instance().ProcessaAllEvent<ThreadCategory::Main>();
	//Main Thread 👇
	renderer_->RequestFrame();
}

EngineContext::~EngineContext() {
	fps_controller_.RequestStop();
	window_.reset();
}

void EngineContext::Quit() {
	window_->Quit();
}

} //namespace Context