#include "ring_buffer.h"

namespace Core::Memory {

template <typename T, size_t Capacity>
bool RingBuffer<T, Capacity>::Push(const T& item) {
	size_t current_write = write_ptr.load(std::memory_order_relaxed);
	size_t next_write = (current_write + 1) & (Capacity - 1);
	if (next_write == read_ptr.load(std::memory_order_acquire)) {
		return false; // FILL
	}
	buffer[current_write] = item;
	write_ptr.store(next_write, std::memory_order_release);
	return true;
}

template <typename T, size_t Capacity>
bool RingBuffer<T, Capacity>::Pop(T& item) {
	size_t current_read = read_ptr.load(std::memory_order_relaxed);
	if (current_read == write_ptr.load(std::memory_order_acquire)) {
		return false; //EMPTY
	}
	item = buffer[current_read];
	read_ptr.store((current_read + 1) & (Capacity - 1), std::memory_order_release);
	return true;
}

template <typename T, size_t Capacity>
size_t RingBuffer<T, Capacity>::PushBulk(const T* data, size_t count) {
	size_t current_write = write_ptr.load(std::memory_order_relaxed);
	size_t current_read = read_ptr.load(std::memory_order_acquire);
	size_t used = current_write - current_read;
	size_t available = Capacity - used - 1;
	size_t to_write = std::min(count, available);
	for (size_t i = 0; i < to_write; ++i) {
		buffer[(current_write + i) & (Capacity - 1)] = data[i];
	}
	write_ptr.store((current_write + to_write) & (Capacity - 1), std::memory_order_release);
	return to_write;
}
template <typename T, size_t Capacity>
size_t RingBuffer<T, Capacity>::PopBulk(T* data, size_t count) {
	size_t current_read = read_ptr.load(std::memory_order_relaxed);
	size_t current_write = write_ptr.load(std::memory_order_acquire);
	size_t available = current_write - current_read;
	size_t to_read = std::min(count, available);
	for (size_t i = 0; i < to_read; ++i) {
		data[i] = buffer[(current_read + i) & (Capacity - 1)];
	}
	read_ptr.store((current_read + to_read) & (Capacity - 1), std::memory_order_release);
	return to_read;
}

// CAS
template <typename T, size_t Capacity>
bool RingBuffer<T, Capacity>::PushCas(const T& item) {
	size_t current_write = write_ptr.load(std::memory_order_relaxed);
	size_t next_write = (current_write + 1) & (Capacity - 1);
	if (next_write == read_ptr.load(std::memory_order_acquire)) {
		return false;
	}

	//CAS
	while (!write_ptr.compare_exchange_weak(
			current_write,
			next_write,
			std::memory_order_release,
			std::memory_order_relaxed)) {
		next_write = (current_write + 1) & (Capacity - 1);
		if (next_write == read_ptr.load(std::memory_order_acquire)) {
			return false;
		}
	}

	buffer[current_write] = item;
	return true;
}

template <typename T, size_t Capacity>
bool RingBuffer<T, Capacity>::PopCas(T& item) {
	size_t current_read = read_ptr.load(std::memory_order_relaxed);
	if (current_read == write_ptr.load(std::memory_order_acquire)) {
		return false;
	}

	// CAS
	while (!read_ptr.compare_exchange_weak(
			current_read,
			(current_read + 1) & (Capacity - 1),
			std::memory_order_release,
			std::memory_order_relaxed)) {
		if (current_read == write_ptr.load(std::memory_order_acquire)) {
			return false;
		}
	}

	item = buffer[current_read];
	return true;
}

template <typename T, size_t Capacity>
size_t RingBuffer<T, Capacity>::PushBulkCas(const T* data, size_t count) {
	size_t current_write;
	size_t current_read;
	size_t to_write;

	do {
		current_write = write_ptr.load(std::memory_order_relaxed);
		current_read = read_ptr.load(std::memory_order_acquire);

		size_t used = current_write - current_read;
		size_t available = Capacity - used - 1;
		to_write = std::min(count, available);

		if (to_write == 0) {
			return 0; // fill
		}

	} while (!write_ptr.compare_exchange_weak(
			current_write,
			(current_write + to_write) & (Capacity - 1),
			std::memory_order_release,
			std::memory_order_relaxed));

	for (size_t i = 0; i < to_write; ++i) {
		buffer[(current_write + i) & (Capacity - 1)] = data[i];
	}

	return to_write;
}

template <typename T, size_t Capacity>
size_t RingBuffer<T, Capacity>::PopBulkCas(T* data, size_t count) {
	size_t current_read;
	size_t current_write;
	size_t to_read;

	do {
		current_read = read_ptr.load(std::memory_order_relaxed);
		current_write = write_ptr.load(std::memory_order_acquire);

		size_t available = current_write - current_read;
		to_read = std::min(count, available);

		if (to_read == 0) {
			return 0; // empty
		}
	} while (!read_ptr.compare_exchange_weak(
			current_read,
			(current_read + to_read) & (Capacity - 1),
			std::memory_order_release,
			std::memory_order_relaxed));

	// read
	for (size_t i = 0; i < to_read; ++i) {
		data[i] = buffer[(current_read + i) & (Capacity - 1)];
	}

	return to_read;
}

} //namespace Core::Memory