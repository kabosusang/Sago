#ifndef SG_MEMORY_LOCKFREE_ARRAY_H
#define SG_MEMORY_LOCKFREE_ARRAY_H
//SPSC
#include <atomic>
#include "core/memory/pool/free_list.h"

namespace Core::Memory{

template <typename T, size_t Capacity = 256>
class LockFreeArray {
public:
	bool push(const T& item) {
		std::size_t tail = tail_.load(std::memory_order_relaxed);

		std::size_t next_tail = (tail + 1) & (capacity_ - 1);

		if (next_tail != head_.load(std::memory_order_acquire)) {
			buffer_[tail] = item;
			tail_.store(next_tail, std::memory_order_release);
			return true;
		}
		return false;
	}

	bool pop(T& item) {
		std::size_t head = head_.load(std::memory_order_relaxed);

		if (head == tail_.load(std::memory_order_acquire)) {
			return false;
		}

		item = buffer_[head];
		head_.store((head + 1) & (capacity_ - 1),std::memory_order_release);
		return true;
	}

private:
	const std::size_t capacity_{ Capacity };
	std::vector<T> buffer_{Capacity};
	std::atomic<std::size_t> head_{ 0 };
	std::atomic<std::size_t> tail_{ 0 };
};







}














#endif