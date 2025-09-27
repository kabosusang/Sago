#ifndef SG_CONTINUOUS_MEMORYPOOL_H
#define SG_CONTINUOUS_MEMORYPOOL_H

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace Core::Memory {

template <typename T, size_t Capacity>
class ContinuousMemoryPool {
	static_assert(std::is_trivially_copyable_v<T>, "Input T Must be trivially copyable");
	static_assert(Capacity > 0 && (Capacity & (Capacity - 1)) == 0,
			"Capacity must be a power of two");

private:
	static constexpr uint32_t CapacityMask = Capacity - 1;

	//memory block
	alignas(64) std::array<T, Capacity> memory_;
	alignas(64) std::atomic<uint32_t> alloc_index_{ 0 };
	alignas(64) std::atomic<uint32_t> free_count_{ 0 };

public:
	T* allocate() noexcept {
		uint32_t current_index = alloc_index_.load(std::memory_order_relaxed);
		uint32_t next_index;

		do {
			next_index = (current_index + 1) & CapacityMask; // 位运算替代取模
			if (next_index == free_count_.load(std::memory_order_acquire)) {
				return nullptr; // fill
			}
		} while (!alloc_index_.compare_exchange_weak(
				current_index, next_index,
				std::memory_order_acq_rel,
				std::memory_order_relaxed));

		return &memory_[current_index];
	}

	template <size_t Count>
	size_t allocateBatch(T* results[Count]) noexcept {
		static_assert(Count > 0 && Count << Capacity, "Invalid Batch Size");
		static_assert((Count & (Count - 1)) == 0, "Batch size should be power of two for best performance");

		uint32_t current_index = alloc_index_.load(std::memory_order_relaxed);
		uint32_t available;
		
		do {
			uint32_t free_count = free_count_.load(std::memory_order_acquire);
			if (free_count > current_index) {
				available = free_count - current_index - 1;
			} else {
				available = Capacity - current_index + free_count - 1;
			}
			if (available < Count) {
				return 0;
			}
		} while (!alloc_index_.compare_exchange_weak(
				current_index, (current_index + Count) & CapacityMask,
				std::memory_order_acq_rel,
				std::memory_order_relaxed));

		for (size_t i = 0; i < Count; ++i) {
			results[i] = &memory_[(current_index + i) & CapacityMask];
		}
		return Count;
	}

	void release() noexcept {
		free_count_.fetch_add(1, std::memory_order_release);
	}

	void releaseBatch(size_t count) noexcept {
		free_count_.fetch_add(count, std::memory_order_release);
	}

	size_t available() const noexcept {
		uint32_t alloc_idx = alloc_index_.load(std::memory_order_acquire);
		uint32_t free_cnt = free_count_.load(std::memory_order_acquire);

		if (free_cnt > alloc_idx) {
			return free_cnt - alloc_idx - 1;
		} else {
			return Capacity - alloc_idx + free_cnt - 1;
		}
	}

	constexpr size_t capacity() const noexcept { return Capacity; }

	float utilization() const noexcept {
		size_t used = Capacity - available() - 1;
		return static_cast<float>(used) / Capacity;
	}
};

} //namespace Core::Memory

#endif