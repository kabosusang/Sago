#ifndef SG_MEMORY_DOUBLE_BUFFER_H
#define SG_MEMORY_DOUBLE_BUFFER_H

#include <atomic>

namespace Core::Memory {

template <typename T>
class LockFreeDoubleBuffer {
public:
	template <typename F>
	void update(F&& writer);
	template <typename F>
	void read(F&& reader) const;

private:
	T buffers[2];
	alignas(64) std::atomic<int> frontIndex{ 0 };
};
} //namespace Core::Memory

#endif