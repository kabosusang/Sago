#ifndef SG_UTIL_SPINK_LOCK_H
#define SG_UTIL_SPINK_LOCK_H
#include <atomic>

namespace Core::util {

static constexpr int kSpinTime = 512;

class SpinLock {
private:
	std::atomic_flag spinlock_ = ATOMIC_FLAG_INIT;

public:
	void Lock();
	void UnLock();
};

} //namespace Core::util

#endif