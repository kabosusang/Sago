#include "runtime.h"

#include "External/AppWindow.hpp"
#include <atomic>

bool Runtime::Init() {
	using namespace platform;
	if (AppWindow::Instance().ShouldExit()) {
		return false;
	}

	LogInfo("地址: {:#x}",reinterpret_cast<uintptr_t>(&Core::Log::AsyncLog::Instance()));
    

	runing_ = true;
    atomic_runing_.store(true,std::memory_order_seq_cst);
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
	SDL_Quit();
}

void Runtime::Pause() {
    atomic_runing_.store(false,std::memory_order_release);
    atomic_runing_.notify_all();
}
