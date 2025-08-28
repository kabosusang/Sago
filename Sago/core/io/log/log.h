#ifndef SG_LOG_H
#define SG_LOG_H

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <format>
#include <iostream>
#include <queue>
#include <string>
#include <string_view>
#include <thread>
#include <utility>

#include "common/single_internal.h"
#include "core/util/spain_lock.h"
#include "core/util/dll_export.h"


namespace Core::Log {

namespace Color {
constexpr auto RESET = "\033[0m";
constexpr auto RED = "\033[0;31mERROR: ";
constexpr auto GREEN = "\033[1;32mINFO: ";
constexpr auto YELLOW = "\033[1;33mWARRING: ";
constexpr auto BLUE = "\033[0;34m";
constexpr auto CANY = "\033[1;36m";


constexpr auto RED_BG = "\033[0;30;41m";
constexpr auto CYAN_BG = "\033[0;30;46m";
} //namespace Color

enum class LogRank {
	kInfo = 0,
	kWarring = 1,
	kError = 2,
	kVulkanLayer = 3
};

enum class LogPolicy : short {
	kSimple = 0,
	kDetail = 1
};


template <LogRank rk>
concept ValidLogRank = (rk == LogRank::kInfo || rk == LogRank::kWarring || rk == LogRank::kError
|| rk == LogRank::kVulkanLayer);

template <LogRank rk>
	requires ValidLogRank<rk>
constexpr auto LogColor = [] {
	if constexpr (rk == LogRank::kInfo) {
		return Color::GREEN;
	} else if constexpr (rk == LogRank::kWarring) {
		return Color::YELLOW;
	} else if constexpr (rk == LogRank::kError){
		return Color::RED;
	} else{
		return Color::CANY;
	}
}();

template <typename... Args>
concept PointLog = (sizeof...(Args) > 0) && (std::is_pointer_v<std::remove_reference_t<Args>> || ...);

consteval auto PolicySelect(LogPolicy po){
	return po;
}


//read log todo..
// class AsyncDoubleBufferLog : public util::DoubleBuffer<AsyncDoubleBufferLog, std::string> {
// public:
// public:
// 	void Push(const std::string& item);
// 	std::vector<std::string>& Swap();
// };

//cmd
class SAGA_API AsyncLog : public Common::Singleton<AsyncLog> {
	friend class Common::Singleton<AsyncLog>;

private:
	std::queue<std::pair<LogPolicy, std::string>> log_queue_;
	std::atomic<bool> running_;
	std::thread consumer_;
	util::SpinLock spinlock_;
	std::atomic<int> msg_count_{0};

	AsyncLog() :
			consumer_(&AsyncLog::LogLoop, this) {
		running_.store(true, std::memory_order_release);
	}

	~AsyncLog() noexcept {
		running_.store(false, std::memory_order_release);
		spinlock_.lock();
		while (!log_queue_.empty()) {
			std::string msg;
			msg = std::move(log_queue_.front().second);
			std::cout << msg;
			log_queue_.pop();
		}
		spinlock_.unlock();
		if (consumer_.joinable()) {
			consumer_.join();
		}
	}
	
	void PrintPolicy(LogPolicy, std::string&&) const;

public:
	void LogLoop();
	void Log(std::string&&);
	void LogDetail(std::string_view, std::string_view, int, std::string&&);
};

template <LogRank rk, typename... Args>
inline void PrintLogFormat(std::format_string<Args...> fmt, Args&&... args) noexcept {
	auto message = LogColor<rk> + std::format(fmt, std::forward<Args>(args)...);
	AsyncLog::Instance().Log(std::move(message));
}

template <LogRank rk, typename... Args>
inline void PrintLogFormatDetail(const char* filename, int codeline, std::format_string<Args...> fmt, Args&&... args) noexcept {
	auto message = std::format(fmt, std::forward<Args>(args)...);
	AsyncLog::Instance().LogDetail(LogColor<rk>, filename, codeline, std::move(message));
	if constexpr (rk == LogRank::kError){
		assert(false);
	}
}

}; //namespace Core::Log

// Defualt Async Log
#define LogInfo(...)                                     \
	Core::Log::PrintLogFormat<Core::Log::LogRank::kInfo>( \
			__VA_ARGS__)

#define LogWarring(...)                                     \
	Core::Log::PrintLogFormat<Core::Log::LogRank::kWarring>( \
			__VA_ARGS__)

#define LogError(...)                                     \
	Core::Log::PrintLogFormat<Core::Log::LogRank::kError>( \
			__VA_ARGS__)

#define LogInfoDetail(...)                                    \
	Core::Log::PrintLogFormatDetail<Core::Log::LogRank::kInfo>( \
			__FILE__, __LINE__, __VA_ARGS__)

#define LogWarringDetail(...)                                    \
	Core::Log::PrintLogFormatDetail<Core::Log::LogRank::kWarring>( \
			__FILE__, __LINE__, __VA_ARGS__)

#define LogErrorDetail(...)                                    \
	Core::Log::PrintLogFormatDetail<Core::Log::LogRank::kError>( \
			__FILE__, __LINE__, __VA_ARGS__)

#endif
