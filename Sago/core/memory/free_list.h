#ifndef SG_MEMORY_FREELIST_H
#define SG_MEMORY_FREELIST_H
#include <cstddef>
#include <new>

#include "core/io/log/log.h"

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include <sys/mman.h>
#include <unistd.h>
#endif

namespace Core::Memoory {

// 4KB
inline static void* SystemAlloc(size_t kpage) {
#if defined(_WIN32)
	void* ptr = VirtualAlloc(0, kpage << 12, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#elif defined(__linux__)
	void* ptr = mmap(0, kpage << 12, PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (ptr == MAP_FAILED) {
		ptr = nullptr;
	}
#endif
	if (ptr == nullptr) {
		try {
			ptr = ::operator new(kpage << 12); // default
		} catch (const std::bad_alloc&) {
			return nullptr;
		}
	}
	return ptr;
}

template <class T, size_t N = 1024>
class ObjectPool {
	char* memory_ = nullptr;
	size_t size_{};
	void* free_list_{};

public:
	void Delete(T* obj) {
		obj->~T();

		*(void**)obj = free_list_; 
		free_list_ = *(void**)obj; 
	}

	T* New() {
		T* obj = nullptr;

		if (free_list_) {
			obj = (T*)free_list_;
			free_list_ = *(void**)obj;
		} else {
			//如果后面的空间大小不够一个T类型对象的大小，那就新开辟一个空间
			if (size_ < sizeof(T)) {
				size_ = 128 * N;
				memory_ = (char*)SystemAlloc(size_);
				if (memory_ == nullptr) {
					LogErrorDetaill("ObjectPool malloc false!");
				}
			}
			obj = (T*)memory_;
			
			size_t objsize = sizeof(T) < sizeof(void*) ? sizeof(void*) : sizeof(T);
			memory_ += objsize;
			size_ -= objsize;
		}

		new (obj) T; //placement new
		return obj;
	}

	void Release() {
		if (memory_) {
#if defined(_WIN32)
			VirtualFree(memory_, 0, MEM_RELEASE);
#elif defined(__linux__)
			munmap(memory_, size_);
#else
    ::operator delete(ptr);
#endif
			memory_ = nullptr;
			size_ = 0;
		}
		free_list_ = nullptr;
	}
};

} //namespace Core::Memoory

#endif