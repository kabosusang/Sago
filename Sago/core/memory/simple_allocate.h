#ifndef SG_MEMORY_ALLOCATE_H
#define SG_MEMORY_ALLOCATE_H
#include <cstddef>
#include <iostream>
#include <limits>
#include <mutex>
#include <vector>


template <class T>
class MyAllocator {
public:
	using value_type = T;
	using pointer = T*;
	using const_pointer = const T*;
	using void_pointer = void*;
	using const_void_pointer = const void*;
	using size_type = size_t;
	using difference_type = std::ptrdiff_t;
	MyAllocator() = default;
	~MyAllocator() = default;

public:
	template <class U>
	struct rebind {
		using other = MyAllocator<U>;
	};

	pointer allocate(size_type numObjects) {
		allocCount += numObjects;
		return static_cast<pointer>(operator new(sizeof(T) * numObjects));
	}

	pointer allocate(size_type numObjects, const_void_pointer hint) {
		return allocate(numObjects);
	}

	void deallocate(pointer p, size_type numObjects) {
		std::cout << "MyAllocator::deallocate,内存释放:" << numObjects << std::endl;
		allocCount = allocCount - numObjects;
		operator delete(p);
	}

	size_type max_size() const {
		return (std::numeric_limits<size_type>::max)();
	}

	size_type get_allocations() const {
		return allocCount;
	}

private:
	size_type allocCount;
};

template <class T>
class PoolAllocator {
    struct MemoryBlock {
        T* data;                  
        size_t used = 0;       
        MemoryBlock* next = nullptr;
    };

    static inline thread_local MemoryBlock* tls_block = nullptr;  // 线程本地块
    static inline std::mutex global_mutex;
    static inline std::vector<MemoryBlock*> global_pool;         // 全局备用池
};


/*
pointer allocate(size_type n) {

    if (!tls_block || tls_block->used + n > BLOCK_SIZE) {
        tls_block = GetOrCreateBlock(); 
    }
    
    T* ptr = &tls_block->data[tls_block->used];
    tls_block->used += n;
    return ptr;
}

void deallocate(pointer p, size_type n) {
    MemoryBlock* block = FindBlockContaining(p);
    
    block->used -= n;
    

    if (block->used == 0) {
        std::lock_guard lock(global_mutex);
        global_pool.push_back(block);
    }
}


using TransformVector = std::vector<Transform, PoolAllocator<Transform>>;
TransformVector transforms; 


MemoryBlock* GetOrCreateBlock() {
    {
        std::lock_guard lock(global_mutex);
        if (!global_pool.empty()) {
            auto* block = global_pool.back();
            global_pool.pop_back();
            return block;
        }
    }
    
  
    auto* block = new MemoryBlock;
    block->data = static_cast<T*>(::operator new(BLOCK_SIZE * sizeof(T)));
    return block;
}


*/









#endif