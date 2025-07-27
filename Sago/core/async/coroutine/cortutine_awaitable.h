#ifndef SG_COROUTINE_AWAITABLE_H
#define SG_COROUTINE_AWAITABLE_H

#include <coroutine>
#include <optional>
#include <functional>

#include "core/io/log/log.h"

namespace Core::Async{

template <typename ConcreteReader,typename T, typename ValuePolicy = std::optional<T>>
class AwaitReader {
public:
    using CompletionCallback = std::function<void(const T&)>;

    explicit AwaitReader(CompletionCallback callback = nullptr)
        : completion_callback_(std::move(callback)) {}

    bool await_ready() const noexcept { 
        return static_cast<const ConcreteReader*>(this)->await_ready();
    }

    void await_suspend(std::coroutine_handle<> handle) {
        saved_handle_ = handle;
        static_cast<const ConcreteReader*>(this)->await_suspend(handle);
    }

    T await_resume() {
        if (!value_policy_.has_value()) {
            LogErrorDetaill("Await resume No value available");
            return {};
        }
        return std::move(value_policy_.value());
    }

    void notify_data_ready(T value) {
        value_policy_.emplace(std::move(value));
        if (saved_handle_ && !saved_handle_.done()) {
            saved_handle_.resume();
        }
        if (completion_callback_) {
            completion_callback_(value_policy_.value());
        }
    }

    bool has_value() const noexcept { return value_policy_.has_value(); }
    bool is_done() const noexcept { return saved_handle_.done(); }

    void reset() {
        value_policy_.reset();
        saved_handle_ = nullptr;
    }

private:
    ValuePolicy value_policy_;    
    std::coroutine_handle<> saved_handle_;
    CompletionCallback completion_callback_;
};


}


#endif