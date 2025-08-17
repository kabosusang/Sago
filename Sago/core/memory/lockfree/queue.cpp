#include "queue.h"

/*
	std::atomic<Node*> head_;
	std::atomic<Node*> tail_;
*/

namespace Core::Memory {

template <typename T>
void LockFreeQueue<T>::Enqueue(T newdata) {
	Node* newNode = new Node(newdata);
	Node* oldTail = tail_.load();
	while (!tail_.compare_exchange_weak(oldTail, newNode)) {
	}
	oldTail->next = newNode;
}

template <typename T>
std::shared_ptr<T> LockFreeQueue<T>::Dequeue() {
	Node* oldHead = head_.load();
	while (oldHead && !head_.compare_exchange_weak(oldHead, oldHead->next)) {
		// 循环直到头部指针更新成功
	}
	return oldHead ? oldHead->data : std::shared_ptr<T>();
}



//Pool
template <typename T>
void LockFreeQueuePool<T>::Enqueue(T newdata) {
	Node* newNode = pool_.New();
	Node* oldTail = tail_.load();
	while (!tail_.compare_exchange_weak(oldTail, newNode)) {
	}
	oldTail->next = newNode;
}

template <typename T>
std::shared_ptr<T> LockFreeQueuePool<T>::Dequeue() {
	Node* oldHead = head_.load();
	while (oldHead && !head_.compare_exchange_weak(oldHead, oldHead->next)) {
		// 循环直到头部指针更新成功
	}
    pool_.Delete(oldHead);
	return oldHead ? oldHead->data : std::shared_ptr<T>();
}

template <typename T>
LockFreeQueuePool<T>::~LockFreeQueuePool<T>(){
    pool_.Release();
}


} //namespace Core::Memory
