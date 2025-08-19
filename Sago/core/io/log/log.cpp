#include "log.h"

#include <atomic>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <mutex>
#include <sstream>

namespace Core::Log {

void AsyncLog::PrintPolicy(LogPolicy policy, std::string&& str) const {
	switch (policy) {
		case LogPolicy::kSimple: {
			std::cout << str + Color::RESET + '\n';
		} break;
		case LogPolicy::kDetail: {
			auto now = std::chrono::system_clock::now();
			auto in_time_t = std::chrono::system_clock::to_time_t(now);
			std::tm tm_buf;

#ifdef _WIN32
			localtime_s(&tm_buf, &in_time_t);
#else
			localtime_r(&in_time_t, &tm_buf);
#endif
			std::ostringstream ss;
			ss << std::put_time(&tm_buf, "[%Y-%m-%d %H:%M:%S]");
			std::cout << str + "---------------->" + ss.str() + Color::RESET + '\n';
		} break;
	}
}

void AsyncLog::LogLoop() {
	while (running_.load(std::memory_order_acquire)) {
		msg_count_.wait(0, std::memory_order_acquire);
		{
			std::unique_lock lock(spinlock_);
			while (!log_queue_.empty()) {
				PrintPolicy(log_queue_.front().first, std::move(log_queue_.front().second));
				log_queue_.pop();
			}
		}
		msg_count_.store(0, std::memory_order_release);
	}
}

void AsyncLog::Log(std::string&& str) {
	{
		std::unique_lock lock(spinlock_);
		log_queue_.push({ LogPolicy::kSimple, std::move(str) });
	}

	if (msg_count_.fetch_add(1, std::memory_order_release) == 0) {
		msg_count_.notify_one();
	}
}

void AsyncLog::LogDetail(std::string_view color, std::string_view filename, int codeline, std::string&& str) {
	{
		std::unique_lock lock(spinlock_);
		auto msg = std::format("{}[{}:{}] {}",
				color,
				std::filesystem::path(filename).filename().string(),
				codeline,
				str);
		log_queue_.push({ LogPolicy::kDetail, std::move(msg) });
	}

	if (msg_count_.fetch_add(1, std::memory_order_release) == 0) {
		msg_count_.notify_one();
	}
}

} //namespace Core::Log