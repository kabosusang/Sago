#ifndef SG_CONXTEX_FRAMERATE_CONTROLLER_H
#define SG_CONXTEX_FRAMERATE_CONTROLLER_H

#include <chrono>
#include <mutex>

#include "core/io/log/log.h"

namespace Context::Controller {

class FrameRateController {
public:
	explicit FrameRateController(int fps) :
			frameDuration_(std::chrono::nanoseconds(1'000'000'000 / fps)) {}
public:
	void GetCurrentFPS_Log() const;
public:
	const void StartFrame() const;
	const void EndFrame();
public:
	bool ShouldContinue() const;
	void RequestStop();
	float GetAverageFPS()const;
	inline void SetFrameRate(int fps) { frameDuration_ = std::chrono::nanoseconds(1'000'000'000 / fps); }

private:
	struct ThreadLocalData {
		std::chrono::steady_clock::time_point frameStart;
		std::chrono::nanoseconds lastDuration{ 0 };
		long long frameCount = 0;
	};

	ThreadLocalData& GetThreadLocalData() const {
		thread_local ThreadLocalData tls;
		return tls;
	}
	std::chrono::nanoseconds frameDuration_;
	std::atomic<bool> stopRequested_{ false };

	//Average
	mutable std::mutex statsMutex_;
	float totalFrames_ = 0;
	std::chrono::duration<float> totalTime_{ 0 };
};

} //namespace Context::Controller

#endif