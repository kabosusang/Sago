#ifndef SG_MEMORY_LOCKFREE_QUEUE_H
#define SG_MEMORY_LOCKFREE_QUEUE_H

#include <atomic>
#include <memory>

#include "core/memory/pool/free_list.h"

namespace Core::Memory {

template <typename T>
class LockFreeQueue {
private:
	struct Node {
		std::shared_ptr<T> data_;
		std::atomic<Node*> next_;
		Node(T newData) :
				data_(std::make_shared<T>(newData)), next_(nullptr) {}
	};

    std::atomic<Node*> head_;
	std::atomic<Node*> tail_;
public:
    void Enqueue(T newdata);
    std::shared_ptr<T> Dequeue();
};


template <typename T>
class LockFreeQueuePool{
private:
	struct Node {
		std::shared_ptr<T> data_;
		std::atomic<Node*> next_;
		Node(T newData) :
				data_(std::make_shared<T>(newData)), next_(nullptr) {}
	};
    
    std::atomic<Node*> head_;
	std::atomic<Node*> tail_;
    ObjectPool<Node> pool_;
    ~LockFreeQueuePool();
public:
    void Enqueue(T newdata);
    std::shared_ptr<T> Dequeue();
};





} //namespace Core::Memoory

#endif