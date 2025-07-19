#ifndef SG_UTIL_DOUBLE_BUFFER_H
#define SG_UTIL_DOUBLE_BUFFER_H
#include <atomic>
#include <cstddef>
#include <vector>

#include "spain_lock.h"

namespace Core::util {
template <typename T>
class DoubleBuffer {
	std::vector<T> buffers_[2];
	std::atomic<size_t> idx_{ 0 };
	SpinLock spinlock_;

public:
	virtual void Push(const T& item) = 0;
	virtual std::vector<T> Swap() = 0;
};

} //namespace Core::util

#endif