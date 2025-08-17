#ifndef SG_MEMORY_BUFFER_RINGBUFFER_H
#define SG_MEMORY_BUFFER_RINGBUFFER_H

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <memory>

namespace Core::Memory {

template <typename T, size_t Capacity>
class RingBuffer {
public:
	RingBuffer() :
			buffer(std::make_unique<T[]>(Capacity)) {
		static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be a power of two");
	}
	bool Push(const T& item);
	bool Pop(T& item);
	size_t PushBulk(const T* data, size_t count);
	size_t PopBulk(T* data, size_t count);
	//CAS
	bool PushCas(const T& item);
	bool PopCas(T& item);
	size_t PushBulkCas(const T* data, size_t count);
	size_t PopBulkCas(T* data, size_t count);

private:
	std::unique_ptr<T[]> buffer;
	alignas(64) std::atomic<size_t> read_ptr{ 0 };
	alignas(64) std::atomic<size_t> write_ptr{ 0 };
};

} //namespace Core::Memory

#endif