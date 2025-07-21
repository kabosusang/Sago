#include "log.h"

#include <atomic>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <sstream>

namespace Core::Log {

void AsyncLog::PrintPolicy(LogPolicy policy, std::string&& str) const {
	switch (policy) {
		case LogPolicy::kSimple: {
			std::cout << str + Color::RESET + '\n';
		}break;
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
		spinlock_.Lock();
		if (!log_queue_.empty()) {
			PrintPolicy(log_queue_.front().first, std::move(log_queue_.front().second));
			log_queue_.pop();
		}
		spinlock_.UnLock();
	}
}

void AsyncLog::Log(std::string&& str) {
	spinlock_.Lock();
	log_queue_.push({ LogPolicy::kSimple, std::move(str) });
	spinlock_.UnLock();
}

void AsyncLog::LogDetail(std::string_view color, std::string_view filename, int codeline, std::string&& str) {
	//todo Mutli Load
	spinlock_.Lock();
	auto msg = std::format("{}[{}:{}] {}",
			color,
			std::filesystem::path(filename).filename().string(),
			codeline,
			str);

	log_queue_.push({ LogPolicy::kDetail, std::move(msg) });
	spinlock_.UnLock();
}

} //namespace Core::Log