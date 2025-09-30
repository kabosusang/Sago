#include "runtime.h"

#include "context/engine_context.h"

#include <atomic>

bool Runtime::Init() {
	runing_ = true;
	atomic_runing_.store(true, std::memory_order_release);
	return true;
}

void Runtime::Tick() {
	auto& Engine = Context::EngineContext::Instance();
	Engine.StartFrame();
	Context::EngineContext::Instance().Tick();

	if (++check_runing_framcount_ >= 200) {
		check_runing_framcount_ = 0;
		if (!atomic_runing_.load(std::memory_order_acquire)) {
			runing_ = false;
		}
	}

	Engine.EndFrame();
}

void Runtime::Quit() {
	Context::EngineContext::Instance().Quit();
}

void Runtime::Pause() {
	atomic_runing_.store(false, std::memory_order_release);
	atomic_runing_.notify_all();
}
