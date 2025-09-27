#ifndef SG_TIME_UTILITY_H
#define SG_TIME_UTILITY_H
#include <cstdint>
#include <chrono>
#include <string>

namespace Core::Time{


class TimeUtility {
public:
    static uint64_t GetUtcTimestamp() {
        using namespace std::chrono;
        auto now = utc_clock::now();
        return duration_cast<milliseconds>(now.time_since_epoch()).count();
    }

    static uint64_t GetMonotonicTimestamp() {
        using namespace std::chrono;
        static auto start_time = steady_clock::now();
        auto now = steady_clock::now();
        return duration_cast<nanoseconds>(now - start_time).count();
    }

    static std::string TimestampToString(uint64_t timestamp_ms);

    static uint64_t GetFrameTimestamp();
    static void Sleep(uint64_t milliseconds);
};






}




#endif