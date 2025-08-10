#ifndef SG_MEMORY_THREAD_POOL_H
#define SG_MEMORY_THREAD_POOL_H

#include <algorithm>
#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <ctime>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

namespace Core::Memoory {
struct Task {
	int priority_{};

	std::function<void()> excute_task_;
	Task() = default;
	Task(std::function<void()>&& task) :
			excute_task_(task) {}
	Task(std::function<void()>&& task, int priority) :
			excute_task_(task), priority_(priority) {}
};

struct Job {
	Task task_{};

	time_t create_time_{};

	bool operator<(const Job& other) const;
	Job(Task task) :
			task_(std::move(task)) {
		this->create_time_ = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	}
};

inline bool Job::operator<(const Job& other) const {
	return this->task_.priority_ == other.task_.priority_
			? this->create_time_ > other.create_time_
			: this->task_.priority_ < other.task_.priority_;
}

class ThreadPool {
private:
	std::priority_queue<Job> task_queue_{};

	std::vector<std::thread> thread_list_{};

	std::mutex thread_lock_{};

	std::condition_variable cv_;

	std::atomic<bool> is_stop_{ false };

	bool is_started_{ false };

	size_t thread_num_{ 0 };

public:
	ThreadPool(ThreadPool&& other) = delete;
	ThreadPool(const ThreadPool& other) = delete;
	void operator=(ThreadPool&&) = delete;

	static ThreadPool& get_thread_pool(size_t thread_num) {
		static ThreadPool pool{ thread_num };
		return pool;
	}

	ThreadPool(size_t thread_num) :
			thread_num_(thread_num) {}
	~ThreadPool() noexcept {
		if (!is_stop_) {
			force_stop_gracefully();
		}
	}

private:
	auto get_execute_func() {
		return [&]() {
			while (true) {
				if (is_stop_) {
					return;
				}

				std::unique_lock<std::mutex> lock(thread_lock_);

				while (task_queue_.empty()) {
					cv_.wait(lock, [&] { return !task_queue_.empty() || is_stop_; });

					if (is_stop_) {
						return;
					}
				}

				auto job = task_queue_.top();

				task_queue_.pop();

				lock.unlock();
				try {
					job.task_.excute_task_();
				} catch (const std::exception& e) {
					std::cerr << "Caught an exception in ThreadPool: " << e.what() << std::endl;
				} catch (...) {
					std::cerr << "Caught an unknown exception in ThreadPool." << std::endl;
				}
			}
		};
	}

	//Main Thread Init
	void add_thread_unsafe(size_t thread_num) {
		for (int i = 0; i < thread_num; i++) {
			auto execute_func = get_execute_func();
			thread_list_.emplace_back(execute_func);
		}
	}

	void add_thread_safe(size_t thread_num) {
		std::unique_lock<std::mutex> lock(thread_lock_);
		thread_num_ += thread_num;

		for (int i = 0; i < thread_num; i++) {
			auto execute_func = get_execute_func();
			thread_list_.emplace_back(std::move(execute_func));
		}
	}

public:
	void add_task_unsafe(std::function<void()>&& task, int priority = 0) {
		task_queue_.push({ { std::move(task), priority } });
		cv_.notify_one();
	}

	void add_task(std::function<void()>&& task, int priority = 0) {
		std::lock_guard<std::mutex> lock(thread_lock_);
		task_queue_.push({ { std::move(task), priority } });
		cv_.notify_one();
	}

	void start() {
		if (is_started_) {
			throw std::runtime_error("the thread pool already started...");
		}
		is_started_ = true;
		add_thread_unsafe(thread_num_);
	}

	void sync() {
		std::for_each(thread_list_.begin(), thread_list_.end(), [](std::thread& t) { t.join(); });
	}

	void force_stop() {
		is_started_ = false;
		if (is_stop_) {
			throw std::runtime_error("thread pool already shutdown!");
		}
		for (auto& thread : thread_list_) {
			thread.detach();
		}
	}

	void force_stop_gracefully() {
		is_started_ = false;
		is_stop_ = true;
		this->sync();
	}

	[[nodiscard]] size_t get_cur_thread_num() const {
		return thread_num_;
	}

	[[nodiscard]] size_t get_task_queue_size() {
		return this->task_queue_.size();
	}
};

} //namespace Core::Memoory

#endif