#ifndef SG_UTIL_DOUBLE_BUFFER_H
#define SG_UTIL_DOUBLE_BUFFER_H
#include <atomic>
#include <cstddef>
#include <vector>

#include "spain_lock.h"

namespace Core::util {
template <typename U,typename T>
class DoubleBuffer {
	std::vector<T> buffers_[2];
	std::atomic<size_t> idx_{ 0 };
	SpinLock spinlock_;

protected:
	void interface(const T& item){
		static_cast<U*>(this)->Push();
	}

	std::vector<T>& interface(){
		static_cast<U*>(this)->Swap();
	}
};

} //namespace Core::util

#endif