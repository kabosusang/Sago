#include "runtime.h"
#include "context/engine_context.h"

#include <atomic>

bool Runtime::Init() {
	auto& context = Context::EngineContext::Instance();

	runing_ = true;
    atomic_runing_.store(true,std::memory_order_release);
	return true;
}

void Runtime::Tick() {
	


	if (++check_runing_framcount_ >= 30) {
		check_runing_framcount_ = 0;
		if (!atomic_runing_.load(std::memory_order_acquire)) {
            runing_ = false;
        }
	}
}

void Runtime::Quit() {
	auto& context = Context::EngineContext::Instance();
	context.Quit();
}

void Runtime::Pause() {
    atomic_runing_.store(false,std::memory_order_release);
    atomic_runing_.notify_all();
}
