#ifndef SG_COROUTINE_BASE_H
#define SG_COROUTINE_BASE_H

#include <coroutine>
#include <memory>

#include "core/async/async_tag.h"
#include "core/io/log/log.h"

namespace Core::Async {
template <typename T>
concept ValidCoroutinePolicy =
		std::is_same_v<T, CoroutinePolicy::Await> ||
		std::is_same_v<T, CoroutinePolicy::Generator>;

template <typename T, ValidCoroutinePolicy tag>
struct CoroutineGenerator;

template <typename T>
struct CoroutineGenerator<T, CoroutinePolicy::Generator> {
	struct promise_type {
		T value_;
		static auto get_return_object_on_allocation_failure() {
			LogErrorDetaill("Error: Failure to Allocation promise_type --> Coroutine");
			return CoroutineGenerator<T, CoroutinePolicy::Generator>{ nullptr };
		}

		auto get_return_object() { return CoroutineGenerator<T, CoroutinePolicy::Generator>{ handle::from_promise(*this) }; }
		auto initial_suspend() { return std::suspend_always{}; }
		auto final_suspend() noexcept { return std::suspend_always{}; }
		void unhandled_exception() { LogErrorDetaill("Error: Failure to  unhandled_exception --> Coroutine"); }
		void return_void() {}
		auto yield_value(T value) {
			this->value_ = value;
			return std::suspend_always{};
		}
	};
	using handle = std::coroutine_handle<promise_type>;

private:
	handle hCoroutine;
	CoroutineGenerator<T, CoroutinePolicy::Generator>(handle handle) :
			hCoroutine(handle) {}

public:
	//int result;
	CoroutineGenerator<T, CoroutinePolicy::Generator>(CoroutineGenerator&& other) noexcept :
			hCoroutine(other.hCoroutine) { other.hCoroutine = nullptr; }
	~CoroutineGenerator<T, CoroutinePolicy::Generator>() {
		if (hCoroutine) {
			hCoroutine.destroy();
		}
	}
	bool MoveNext() const { return hCoroutine && (hCoroutine.resume(), !hCoroutine.done()); }
	T GetValue() const { return hCoroutine.promise().value; }

	auto GetNative() const { return hCoroutine.address(); }
};

template <typename T>
struct CoroutineGenerator<T, CoroutinePolicy::Await> {
	struct promise_type {
		promise_type() :
				value_(std::make_shared<T>()) {}

		static auto get_return_object_on_allocation_failure() {
			LogErrorDetaill("Error: Failure to Allocation promise_type --> Coroutine");
			return CoroutineGenerator<T, CoroutinePolicy::Await>{ nullptr };
		}

		auto get_return_object() { return CoroutineGenerator<T, CoroutinePolicy::Await>{ handle::from_promise(*this) }; }
		auto initial_suspend() { return std::suspend_never{}; }
		auto final_suspend() noexcept { return std::suspend_never{}; }
		void unhandled_exception() { LogErrorDetaill("Error: Failure to  unhandled_exception --> Coroutine"); }
		void return_value(T value) { *value_ = value; }

		// auto yield_value(T value) {
		// 	this->value_ = value;
		// 	return std::suspend_always{};
		// }
	private:
		std::shared_ptr<T> value_;
	};
	using handle = std::coroutine_handle<promise_type>;

private:
	handle hCoroutine;
	CoroutineGenerator<T, CoroutinePolicy::Await>(handle handle) :
			hCoroutine(handle) {}

public:
	CoroutineGenerator<T, CoroutinePolicy::Await>(CoroutineGenerator&& other) noexcept :
			hCoroutine(other.hCoroutine) { other.hCoroutine = nullptr; }
	~CoroutineGenerator<T, CoroutinePolicy::Await>() {
		if (hCoroutine) {
			hCoroutine.destroy();
		}
	}

	T GetValue() const {
		if (!hCoroutine || hCoroutine.done()) {
			LogWarring("Coroutine is invalid or finished");
		}
		return hCoroutine.promise().value_;
	}
	auto GetNative() const { return hCoroutine.address(); }
};

} //namespace Core::Async

#endif