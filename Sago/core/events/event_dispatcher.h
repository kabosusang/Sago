#ifndef SG_EVENT_DISPATCHER_H
#define SG_EVENT_DISPATCHER_H
#include "event_coder.h"
#include <array>
#include <atomic>
#include <cstddef>
#include <tuple>
#include <algorithm>
#include <utility>
#include <variant>

#include "context/renderer/event/renderer_event_type.h"
namespace Core::Event {
//MPSC
template <typename... EventTypes>
class EventDispatcher {
private:
	using EventVariant = std::variant<EventTypes...>;
	using EventMap = EventTypeMap<EventVariant>;

	static constexpr size_t MAX_HANDLERS_PER_TYPE = 32;
	static constexpr size_t MAX_QUEUE_SIZE = 1024;

	static_assert((MAX_QUEUE_SIZE & (MAX_QUEUE_SIZE - 1)) == 0, "MAX_QUEUE_SIZE must be power of 2");
	static constexpr size_t QUEUE_MASK = MAX_QUEUE_SIZE - 1;

private:
	template <typename Event>
	using HandlerArray = std::array<void (*)(const Event&), MAX_HANDLERS_PER_TYPE>;

	std::tuple<HandlerArray<EventTypes>...> handlers_;
	std::array<size_t, sizeof...(EventTypes)> handler_counts_{};

	//Right Buffer
	std::array<EventVariant, MAX_QUEUE_SIZE> event_queue_;
	std::atomic<size_t> queue_head_{ 0 };
	std::atomic<size_t> queue_tail_{ 0 };
	std::atomic<bool> processing_{ false };

	EventCoder<EventVariant> sdl_encoder_;

public:
	template <typename Event>
	bool subscribe(void (*handler)(const Event&)) {
		static_assert((std::is_same_v<Event, EventTypes> || ...),
				"Event type not registered");

		constexpr size_t idx = EventMap::template index_of<Event>();
		static_assert(idx < sizeof...(EventTypes),"Event index out of range");
		
		size_t& count = handler_counts_[idx];

		if (count < MAX_HANDLERS_PER_TYPE) {
			std::get<idx>(handlers_)[count++] = handler;
			return true;
		}
		return false;
	}

	template <typename Event>
	bool publish(Event&& event) {
		size_t head = queue_head_.load(std::memory_order_acquire);
		size_t tail = queue_tail_.load(std::memory_order_relaxed);

		if ((tail - head) == MAX_QUEUE_SIZE) {
			return false; // fill
		}

		event_queue_[tail & QUEUE_MASK] = std::forward<Event>(event);
		queue_tail_.store(tail + 1, std::memory_order_release);
		return true;
	}

	//Batch
	template <typename... Events>
	size_t publishBulk(Events&&... events) {
		size_t published = 0;
		(void)((publish(std::forward<Events>(events)) && (++published, true)) && ...);
		return published;
	}

	bool publishSDL(const SDL_Event& sdl_event) {
		if (auto variant = sdl_encoder_.encode(sdl_event)) {
			return publish(std::move(*variant));
		}
		return false;
	}

	void processAllEvents() {
		if (processing_.exchange(true, std::memory_order_acquire)) {
			return;
		}

		const size_t head = queue_head_.load(std::memory_order_acquire);
		const size_t tail = queue_tail_.load(std::memory_order_acquire);
		const size_t count = tail - head;

		if (count == 0) {
			processing_.store(false, std::memory_order_release);
			return;
		}

		auto processor = [this](const auto& event) {
			using EventT = std::decay_t<decltype(event)>;
			this->processSingleEvent<EventT>(event);
		};

		for (size_t i = 0; i < count; ++i) {
			size_t index = (head + i) & QUEUE_MASK;
			std::visit(processor, event_queue_[index]);
		}

		queue_head_.store(tail, std::memory_order_release);
		processing_.store(false, std::memory_order_release);
	}

	size_t processUpTo(size_t max_events) {
		if (processing_.exchange(true, std::memory_order_acquire)) {
			return 0;
		}

		const size_t head = queue_head_.load(std::memory_order_acquire);
		const size_t tail = queue_tail_.load(std::memory_order_acquire);
		const size_t total_count = tail - head;
		const size_t process_count = (std::min)(max_events, total_count);

		auto processor = [this](const auto& event) {
			using EventT = std::decay_t<decltype(event)>;
			this->processSingleEvent<EventT>(event);
		};

		for (size_t i = 0; i < process_count; ++i) {
			size_t index = (head + i) & QUEUE_MASK;
			std::visit(processor, event_queue_[index]);
		}

		queue_head_.store(head + process_count, std::memory_order_release);
		processing_.store(false, std::memory_order_release);

		return process_count;
	}

	size_t getQueueSize() const {
		size_t head = queue_head_.load(std::memory_order_acquire);
		size_t tail = queue_tail_.load(std::memory_order_acquire);
		return tail - head;
	}

	bool isQueueFull() const {
		return getQueueSize() >= MAX_QUEUE_SIZE;
	}

	bool isQueueEmpty() const {
		return getQueueSize() == 0;
	}

	static constexpr size_t getMaxQueueSize() { return MAX_QUEUE_SIZE; }
	static constexpr size_t getMaxHandlersPerType() { return MAX_HANDLERS_PER_TYPE; }

private:
	template <typename Event>
	void processSingleEvent(const Event& event) {
		constexpr size_t idx = EventMap::template index_of<Event>();
		static_assert(idx < sizeof...(EventTypes), "Event index out of range");
        
		
		const auto& handler_array = std::get<idx>(handlers_);
		const size_t count = handler_counts_[idx];

		if (count == 0) [[unlikely]] {
			return;
		}

		for (size_t i = 0; i < count; ++i) {
			if (handler_array[i]) [[likely]] {
				handler_array[i](event);
			}
		}
	}
};

using GameEventDispatcher = EventDispatcher<
    WindowResizeEvent,
    WindowMinimizeEvent,
    KeyEvent,
    MouseMotionEvent,
    MouseButtonEvent
>;


using RendererEventDispatcher = EventDispatcher<
    Context::Renderer::Event::SwapchainRecreateEvent  ,
    Context::Renderer::Event::RenderFrameEvent
>;












} //namespace Core::Event

#endif