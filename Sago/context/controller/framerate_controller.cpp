#include "framerate_controller.h"

#include <thread>

namespace Context::Controller {

const void FrameRateController::StartFrame() const {
	auto& tls = GetThreadLocalData();
	tls.frameStart = std::chrono::steady_clock::now();
}

const void FrameRateController::EndFrame() {
	auto& tls = GetThreadLocalData();
	auto now = std::chrono::steady_clock::now();
	auto frameTime = now - tls.frameStart;
	auto sleepTime = frameDuration_ - frameTime;

	if (sleepTime > std::chrono::milliseconds(0)) {
		//Accurate Frame
		// auto wakeTime = now + sleepTime;
		// while (std::chrono::steady_clock::now() < wakeTime) {
		// 	std::this_thread::yield();
		// }
		std::this_thread::sleep_until(now + sleepTime);
	}

	tls.lastDuration = std::chrono::steady_clock::now() - tls.frameStart;
	tls.frameCount++;

#if defined(DEBUG)

#else
	{
		std::lock_guard lock(statsMutex_);
		totalTime_ += std::chrono::duration<float>(tls.lastDuration);
		totalFrames_ += 1.0f;
	}
#endif
}

bool FrameRateController::ShouldContinue() const {
	return !stopRequested_.load(std::memory_order_acquire);
}

void FrameRateController::RequestStop() {
	stopRequested_.store(true, std::memory_order_release);
}

float FrameRateController::GetAverageFPS() const {
	std::lock_guard lock(statsMutex_);
	if (totalTime_.count() <= 0) {
		return 0.0f;
	}
	return totalFrames_ / totalTime_.count();
}

void FrameRateController::GetCurrentFPS_Log() const{
	static auto lastLogTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();
	if (std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastLogTime).count() >= 1) {
		float avgFPS = GetAverageFPS();
		LogInfo("FPS: {:.2f}", avgFPS);
		lastLogTime = currentTime;
	}
}

} //namespace Context::Controller