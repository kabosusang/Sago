#ifndef SG_LOG_H
#define SG_LOG_H

#include <atomic>
#include <condition_variable>
#include <format>
#include <queue>
#include <string>
#include <iostream>
#include <string_view>
#include <thread>
#include <utility>

#include "core/util/double_buffer.h"
#include "core/util/spain_lock.h"
#include "core/tools/single_internal.h"


namespace Core::Log {

namespace Color {
constexpr auto RESET = "\033[0m";
constexpr auto RED = "\033[0;31mERROR: ";
constexpr auto GREEN = "\033[1;32mINFO: ";
constexpr auto YELLOW = "\033[1;33mWARRING: ";
constexpr auto BLUE = "\033[0;34m";

constexpr auto RED_BG = "\033[0;30;41m";
constexpr auto CYAN_BG = "\033[0;30;46m";
} //namespace Color

enum class LogRank {
	Info = 0,
	Warring = 1,
	Error = 2
};

template <LogRank rk>
concept ValidLogRank = (rk == LogRank::Info || rk == LogRank::Warring || rk == LogRank::Error);

template <LogRank rk>
	requires ValidLogRank<rk>
constexpr auto LogColor = [] {
	if constexpr (rk == LogRank::Info) {
		return Color::GREEN;
	} else if constexpr (rk == LogRank::Warring) {
		return Color::YELLOW;
	} else {
		return Color::RED;
	}
}();

//read log todo..
class AsyncDoubleBufferLog : public util::DoubleBuffer<std::string> {
public:
public:
	virtual void Push(const std::string& item);
	virtual std::vector<std::string> Swap();
};

//cmd
class AsyncLog : public Tools::Singleton<AsyncLog> {
	friend class Tools::Singleton<AsyncLog>;

private:
	std::queue<std::string> log_queue_;
	std::atomic<bool> running_;
	std::thread consumer_;
	util::SpinLock spinlock_;
	std::condition_variable cv_;
	AsyncLog():
			consumer_(&AsyncLog::LogLoop, this) {
			running_.store(true, std::memory_order_release);
	}

	~AsyncLog() noexcept override {
		running_.store(false, std::memory_order_release);
		spinlock_.Lock();
		while (!log_queue_.empty()){
			std::string msg;
			msg = std::move(log_queue_.front());
			log_queue_.pop();
			std::cout << msg;
		}
		spinlock_.UnLock();
		if (consumer_.joinable()) {
			consumer_.join();
		}
	}

public:
	void LogFormatTime();
	void LogLoop();
	void Log(std::string&&);
	void LogDetail(std::string_view, std::string_view, int, std::string&&);
};

template <LogRank rk, typename... Args>
inline void PrintLogFormat(std::format_string<Args...> fmt, Args&&... args) noexcept{
	auto message = LogColor<rk> + std::format(fmt, std::forward<Args>(args)...);
	AsyncLog::Instance().Log(std::move(message));
}

template <LogRank rk, typename... Args>
inline void PrintLogFormatDetail(const char* filename, int codeline, std::format_string<Args...> fmt, Args&&... args) noexcept{
	auto message = std::format(fmt, std::forward<Args>(args)...);
	AsyncLog::Instance().LogDetail(LogColor<rk>, filename, codeline, std::move(message));
}

}; //namespace Core::Log

// Defualt Async Log
#define LogInfo(...)                                     \
	Core::Log::PrintLogFormat<Core::Log::LogRank::Info>( \
			__VA_ARGS__)

#define LogWarring(...)                                     \
	Core::Log::PrintLogFormat<Core::Log::LogRank::Warring>( \
			__VA_ARGS__)

#define LogError(...)                                     \
	Core::Log::PrintLogFormat<Core::Log::LogRank::Error>( \
			__VA_ARGS__)

#define LogInfoDetaill(...)                                    \
	Core::Log::PrintLogFormatDetail<Core::Log::LogRank::Info>( \
			__FILE__, __LINE__, __VA_ARGS__)

#define LogWarringDetaill(...)                                    \
	Core::Log::PrintLogFormatDetail<Core::Log::LogRank::Warring>( \
			__FILE__, __LINE__, __VA_ARGS__)

#define LogErrorDetaill(...)                                    \
	Core::Log::PrintLogFormatDetail<Core::Log::LogRank::Error>( \
			__FILE__, __LINE__, __VA_ARGS__)

#endif
