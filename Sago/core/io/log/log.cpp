#include "log.h"

#include <atomic>
#include <chrono>
#include <ctime>
#include <iostream>
#include <sstream>
#include <filesystem>


namespace Core::Log {
void AsyncLog::LogLoop() {
	while (running_.load(std::memory_order_acquire)) {
		spinlock_.Lock();
		if (!log_queue_.empty()) {
			std::string msg;
			msg = std::move(log_queue_.front());
			log_queue_.pop();
			std::cout << msg;
		}
		spinlock_.UnLock();
	}
}

void AsyncLog::Log(std::string&& str) {
	spinlock_.Lock();
	log_queue_.push(std::move(str));
	spinlock_.UnLock();
}

void AsyncLog::LogDetail(std::string_view color,std::string_view filename, int codeline, std::string&& str) {
	spinlock_.Lock();

	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);
	std::tm tm_buf;

#ifdef _WIN32
	localtime_s(&tm_buf, &in_time_t);
#else
	localtime_r(&in_time_t, &tm_buf);
#endif

	std::ostringstream ss;
	ss<< color << std::put_time(&tm_buf, "[%Y-%m-%d %H:%M:%S]");
    auto msg = std::format("{} [{}:{}] {}",
                ss.str(),
                std::filesystem::path(filename).filename().string(),
                codeline,
                str);

	log_queue_.push(std::move(msg));
	spinlock_.UnLock();
}

} //namespace Core::Log