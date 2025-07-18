#ifndef SG_LOG_H
#define SG_LOG_H

#include <format>
#include <iostream>
#include <utility>

namespace Core::Log {

namespace Color {
constexpr auto RESET = "\033[0m";
constexpr auto RED = "\033[0;31m";
constexpr auto GREEN = "\033[1;32m";
constexpr auto YELLOW = "\033[1;33m";
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

template <LogRank rk, typename... Args>
inline void PrintLogFormat(std::format_string<Args...> fmt, Args&&... args) {
	auto&& message = std::format(fmt, std::forward<Args>(args)...);
	std::cout << LogColor<rk> << message
			  << Color::RESET << std::endl;
}


}; //namespace Core::Log

#define LogInfo(...)                          \
	Core::Log::PrintLogFormat<Core::Log::LogRank::Info>( \
			__VA_ARGS__);

#define LogWarring(...)                          \
	Core::Log::PrintLogFormat<Core::Log::LogRank::Warring>( \
			__VA_ARGS__);

#define LogError(...)                          \
	Core::Log::PrintLogFormat<Core::Log::LogRank::Error>( \
			__VA_ARGS__);

#endif
