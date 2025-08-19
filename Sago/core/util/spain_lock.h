#ifndef SG_UTIL_SPINK_LOCK_H
#define SG_UTIL_SPINK_LOCK_H
#include <atomic>

namespace Core::util {

static constexpr int kSpinTime = 256;

class SpinLock {
private:
	std::atomic_flag spinlock_ = ATOMIC_FLAG_INIT;
public:
	bool try_lock();
	void lock();
	void unlock();

	SpinLock() = default;
    SpinLock(const SpinLock&) = delete;
    SpinLock& operator=(const SpinLock&) = delete;
};

} //namespace Core::util

#endif