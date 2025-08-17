#include "double_buffer.h"

namespace Core::Memory {

template <typename T>
template <typename F>
void LockFreeDoubleBuffer<T>::update(F&& writer) {
	T& backBuffer = buffers[1 - frontIndex.load(std::memory_order_acquire)];
	writer(backBuffer);
	frontIndex.store(1 - frontIndex.load(), std::memory_order_release);
}

template <typename T>
template <typename F>
void LockFreeDoubleBuffer<T>::read(F&& writer) const {
	const T& frontBuffer = buffers[frontIndex.load(std::memory_order_acquire)];
	reader(frontBuffer);
}

} //namespace Core::Memory