#ifndef SG_CONTEXT_EVENT_H
#define SG_CONTEXT_EVENT_H

#include <functional>
#include <thread>
#include <mutex>

namespace Context::Event{
    
class EventLoop {
public:
    using callable_t = std::function<void()>;

    EventLoop() : m_thread(&EventLoop::threadFunc, this) {}
    EventLoop(const EventLoop&) = delete;
    EventLoop(const EventLoop&&) = delete;
    EventLoop& operator= (const EventLoop&) = delete;
    EventLoop& operator= (const EventLoop&&)  = delete;
    
    ~EventLoop() noexcept {
        stop();
    }

    void enqueue(callable_t&& callable) {
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            m_writeBuffer.emplace_back(std::move(callable));
        }
        m_condVar.notify_one();
    }

    void stop() noexcept {
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            m_running = false;
        }
        m_condVar.notify_all();
        if (m_thread.joinable()) m_thread.join();
    }

private:
    std::vector<callable_t> m_writeBuffer;
    std::mutex m_mutex;
    std::condition_variable m_condVar;
    bool m_running{ true };
    std::thread m_thread;

    void threadFunc() noexcept {
        std::vector<callable_t> readBuffer;
        while (m_running) {
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_condVar.wait(lock, [this] {
                    return !m_running || !m_writeBuffer.empty();
                });
                if (!m_running) break;
                std::swap(readBuffer, m_writeBuffer);
            }
            for (auto& func : readBuffer) func();
            readBuffer.clear();
        }
    }
};




}





#endif