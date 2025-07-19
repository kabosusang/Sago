#include "spain_lock.h"

#if defined(__x86_64__) || defined(_M_X64)
#include <emmintrin.h>
#endif

namespace Core::util {

void SpinLock::Lock() {
	int spins = 0;
	while (spinlock_.test_and_set(std::memory_order_acquire)) {
		if (++spins < kSpinTime) { //short time
#if defined(__x86_64__) || defined(_M_X64)
			_mm_pause();
// #elif defined(__aarch64__)
// 			__asm__ __volatile__("isb sy");
#endif
		} else {
			spinlock_.wait(true, std::memory_order_relaxed);
			spins = 0;
		}
	}
}

void SpinLock::UnLock() {
	spinlock_.clear(std::memory_order_release);
	spinlock_.notify_one();
}

} //namespace Core::util