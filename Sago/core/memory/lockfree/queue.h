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
		std::shared_ptr<T> data;
		Node* next;
		Node() :
				next(nullptr) {}
	};

	std::atomic<Node*> head;
	std::atomic<Node*> tail;
	ObjectPool<Node> pool_;

public:
	LockFreeQueue() :
			head(new Node), tail(head.load()) {}
	~LockFreeQueue() {
		while (Node* const old_head = head.load()) {
			head.store(old_head->next);
			delete old_head;
		}
	}

	void push(T new_value) {
		std::shared_ptr<T> new_data(std::make_shared<T>(std::move(new_value)));
		Node* p = pool_.New();
		Node* const old_tail = tail.load();
		old_tail->data.swap(new_data);
		old_tail->next = p;
		tail.store(p);
	}

	std::shared_ptr<T> pop() {
		Node* old_head = head.load();
		if (old_head == tail.load()) {
			return std::shared_ptr<T>();
		}
		std::shared_ptr<T> const res(old_head->data);
		head.store(old_head->next);
		delete old_head;
		return res;
	}
};

template <typename T>
class LockFreeQueue_Pool {
private:
	struct Node {
		std::shared_ptr<T> data;
		Node* next;
		Node() :
				next(nullptr) {}
	};

	std::atomic<Node*> head;
	std::atomic<Node*> tail;
	ObjectPool<Node> pool_;

public:
	LockFreeQueue_Pool() :
			head(new Node), tail(head.load()) {}
	~LockFreeQueue_Pool() {
		// while (Node* const old_head = head.load()) {
		// 	head.store(old_head->next);
		// 	delete old_head;
		// }
		pool_.Release();
	}

	void push(T new_value) {
		std::shared_ptr<T> new_data(std::make_shared<T>(std::move(new_value)));
		Node* p = pool_.New();
		Node* const old_tail = tail.load();
		old_tail->data.swap(new_data);
		old_tail->next = p;
		tail.store(p);
	}

	std::shared_ptr<T> pop() {
		Node* old_head = head.load();
		if (old_head == tail.load()) {
			return std::shared_ptr<T>();
		}
		std::shared_ptr<T> const res(old_head->data);
		head.store(old_head->next);
		//delete old_head;
		pool_.Delete(old_head);
		return res;
	}
};


template <typename T>
class LockFreeQueue_Cas {
private:
    struct Node {
        std::shared_ptr<T> data;
        std::atomic<Node*> next;
        
        Node() : data(nullptr), next(nullptr) {}  // 哨兵节点构造
        explicit Node(T value) : data(std::make_shared<T>(value)), next(nullptr) {}
    };

    std::atomic<Node*> head;
    std::atomic<Node*> tail;

public:
    LockFreeQueue_Cas() {
        Node* dummy = new Node();  // 创建哨兵节点
        head.store(dummy);
        tail.store(dummy);
    }

    ~LockFreeQueue_Cas() {
        while (Node* node = head.load()) {
            head.store(node->next);
            delete node;
        }
    }

    // 多生产者安全入队
    void push(T value) {
        Node* newNode = new Node(value);
        Node* currTail = tail.load(std::memory_order_relaxed);
        
        while (true) {
            Node* next = currTail->next.load(std::memory_order_acquire);
            if (!next) {
                // 尝试链接新节点
                if (currTail->next.compare_exchange_weak(
                    next, newNode,
                    std::memory_order_release,
                    std::memory_order_relaxed)) {
                    break;
                }
            } else {
                // 帮助其他线程完成尾指针更新
                tail.compare_exchange_weak(
                    currTail, next,
                    std::memory_order_relaxed,
                    std::memory_order_relaxed);
            }
            currTail = tail.load(std::memory_order_relaxed);
        }
        
        // 更新尾指针
        tail.compare_exchange_weak(
            currTail, newNode,
            std::memory_order_release,
            std::memory_order_relaxed);
    }

    // 单消费者出队
    std::shared_ptr<T> pop() {
        Node* currHead;
        Node* currTail;
        Node* next;
        
        while (true) {
            currHead = head.load(std::memory_order_acquire);
            currTail = tail.load(std::memory_order_acquire);
            next = currHead->next.load(std::memory_order_acquire);
            
            if (currHead == currTail) {
                if (!next) return nullptr;  // 队列为空
                // 尾指针落后，帮助更新
                tail.compare_exchange_weak(
                    currTail, next,
                    std::memory_order_relaxed,
                    std::memory_order_relaxed);
            } else {
                if (head.compare_exchange_weak(
                    currHead, next,
                    std::memory_order_release,
                    std::memory_order_relaxed)) {
                    std::shared_ptr<T> res = next->data;
                    delete currHead;  // 安全删除旧头节点
                    return res;
                }
            }
        }
    }

    bool empty() const {
        Node* h = head.load(std::memory_order_acquire);
        Node* t = tail.load(std::memory_order_acquire);
        Node* n = h->next.load(std::memory_order_acquire);
        return (h == t) && (n == nullptr);
    }
};






} //namespace Core::Memory

#endif