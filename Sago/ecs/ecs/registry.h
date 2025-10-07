#ifndef SG_ECS_REGISTRY_H
#define SG_ECS_REGISTRY_H
#include "sparse_set.h"
#include "view.h"
#include "group.h"
#include "generator.h"

#include <algorithm>
#include <unordered_map>

namespace SECS {
class registry;

//CRTP
template <typename Derived>
struct storage_base {
	bool contains(entity entt) const {
		return static_cast<const Derived*>(this)->contains_impl(entt);
	}

	void erase(entity entt) {
		static_cast<Derived*>(this)->erase_impl(entt);
	}

	std::size_t size() const {
		return static_cast<const Derived*>(this)->size_impl();
	}

	auto& get_storage() {
		return static_cast<Derived*>(this)->get_storage_impl();
	}

	const auto& get_storage() const {
		return static_cast<const Derived*>(this)->get_storage_impl();
	}
};

template <typename Component>
struct storage_wrapper : storage_base<storage_wrapper<Component>> {
	basic_storage<Component> storage;

	// CRTP impl
	bool contains_impl(entity entt) const { return storage.contains(entt); }
	void erase_impl(entity entt) { storage.erase(entt); }
	std::size_t size_impl() const { return storage.size(); }

	basic_storage<Component>& get_storage_impl() { return storage; }
	const basic_storage<Component>& get_storage_impl() const { return storage; }
};


class type_erased_storage {
private:
	struct storage_concept {
		virtual ~storage_concept() = default;
		virtual bool contains(entity entt) const = 0;
		virtual void erase(entity entt) = 0;
		virtual std::size_t size() const = 0;
		virtual std::unique_ptr<storage_concept> clone() const = 0;
	};

	template <typename T>
	struct storage_model : storage_concept {
		T storage;

		template <typename... Args>
		storage_model(Args&&... args) :
				storage(std::forward<Args>(args)...) {}

		bool contains(entity entt) const override { return storage.contains(entt); }
		void erase(entity entt) override { storage.erase(entt); }
		std::size_t size() const override { return storage.size(); }

		std::unique_ptr<storage_concept> clone() const override {
			return std::make_unique<storage_model>(storage);
		}

		T& get() { return storage; }
		const T& get() const { return storage; }
	};

	std::unique_ptr<storage_concept> storage_;

public:
	type_erased_storage() = default;

	template <typename T>
	type_erased_storage(T&& storage) :
			storage_(std::make_unique<storage_model<std::decay_t<T>>>(std::forward<T>(storage))) {}

	type_erased_storage(const type_erased_storage& other) :
			storage_(other.storage_ ? other.storage_->clone() : nullptr) {}

	type_erased_storage& operator=(const type_erased_storage& other) {
		if (this != &other) {
			storage_ = other.storage_ ? other.storage_->clone() : nullptr;
		}
		return *this;
	}

	type_erased_storage(type_erased_storage&&) = default;
	type_erased_storage& operator=(type_erased_storage&&) = default;

	bool contains(entity entt) const {
		return storage_ ? storage_->contains(entt) : false;
	}

	void erase(entity entt) {
		if (storage_) {
			storage_->erase(entt);
		}
	}

	std::size_t size() const {
		return storage_ ? storage_->size() : 0;
	}

	template <typename T>
	T* get() {
		auto* model = dynamic_cast<storage_model<T>*>(storage_.get());
		return model ? &model->get() : nullptr;
	}

	template <typename T>
	const T* get() const {
		const auto* model = dynamic_cast<const storage_model<T>*>(storage_.get());
		return model ? &model->get() : nullptr;
	}
};

class registry {
private:
	std::vector<std::uint64_t> free_list_;
	std::vector<std::uint16_t> versions_;
	std::uint64_t next_{ 0 };
	std::vector<entity> entities_;
    //crpt wrapper
	std::unordered_map<std::size_t, type_erased_storage> storages_;

	template <typename Component>
	auto& storage() {
		const auto type_id = type<Component>();
		auto it = storages_.find(type_id);
		if (it == storages_.end()) {
			storage_wrapper<Component> wrapper;
			auto [new_it, inserted] = storages_.emplace(type_id, std::move(wrapper));
			it = new_it;
		}

		auto* wrapper = it->second.template get<storage_wrapper<Component>>();
		SECS_ASSERT(wrapper != nullptr, "Storage type mismatch");
		return wrapper->get_storage();
	}

	template <typename Component>
	const auto& storage() const {
		const auto type_id = type<Component>();
		auto it = storages_.find(type_id);
		if (it == storages_.end()) {
			SECS_ASSERT(false, "Storage not found for component");
		}

		const auto* wrapper = it->second.template get<storage_wrapper<Component>>();
		SECS_ASSERT(wrapper != nullptr, "Storage type mismatch");
		return wrapper->get_storage();
	}

public:
	registry() = default;

	entity create() {
		std::uint64_t id;
		if (free_list_.empty()) {
			id = next_++;
			if (entity_id(tombstone) == id) {
				id = next_++;
			}
			if (id >= versions_.size()) {
				versions_.resize(id + 1, 0);
			}
		} else {
			id = free_list_.back();
			free_list_.pop_back();
			++versions_[id];
		}
		entity entt = make_entity(id, versions_[id]);
		entities_.push_back(entt);
		return entt;
	}

	void destroy(entity entt) {
		const auto id = entity_id(entt);
		if (id < versions_.size() && versions_[id] == entity_version(entt)) {
			for (auto& [type_id, storage] : storages_) {
				storage.erase(entt);
			}
			entities_.erase(std::remove(entities_.begin(), entities_.end(), entt), entities_.end());
			++versions_[id];
			free_list_.push_back(id);
		}
	}

	bool valid(entity entt) const noexcept {
		const auto id = entity_id(entt);
		return id < versions_.size() && versions_[id] == entity_version(entt);
	}

	template <typename Component>
	bool has(entity entt) const {
		return valid(entt) && storage<Component>().contains(entt);
	}

	template <typename Component, typename... Args>
	Component& emplace(entity entt, Args&&... args) {
		if (!valid(entt)) {
			SECS_ASSERT(false, "Cannot emplace component to invalid entity");
		}
		return storage<Component>().emplace(entt, std::forward<Args>(args)...);
	}

	template <typename Component>
	Component& emplace(entity entt, Component&& component) {
		if (!valid(entt)) {
			SECS_ASSERT(false, "Cannot emplace component to invalid entity");
		}
		return storage<Component>().emplace(entt, std::forward<Component>(component));
	}

	template <typename Component>
	Component& get(entity entt) {
		if (!valid(entt) || !has<Component>(entt)) {
			SECS_ASSERT(false, "Entity does not have the requested component");
		}
		return storage<Component>().get(entt);
	}

	template <typename Component>
	const Component& get(entity entt) const {
		if (!valid(entt) || !has<Component>(entt)) {
			SECS_ASSERT(false, "Entity does not have the requested component");
		}
		return storage<Component>().get(entt);
	}

	template <typename Component>
	void remove(entity entt) {
		if (valid(entt)) {
			storage<Component>().erase(entt);
		}
	}

	const std::vector<entity>& get_entities() const noexcept {
		return entities_;
	}
    //view
	template <typename... Components>
	auto create_view() {
		return basic_view<Components...>{ *this };
	}
    //group
	template <typename... Components>
	auto create_group() {
		return basic_group<Components...>{ *this };
	}

	template <typename... Comp>
	friend class basic_view;

	template <typename... Comp>
	friend class basic_group;
};


template <typename... Components>
template <typename Comp>
auto& basic_view<Components...>::get_storage() {
	return owner_.template storage<Comp>();
}

template <typename... Components>
template <typename Comp>
const auto& basic_view<Components...>::get_storage() const {
	return owner_.template storage<Comp>();
}

template <typename... Components>
basic_view<Components...>::basic_view(registry& reg) :
		owner_(reg) {}

template <typename... Components>
template <typename Func>
void basic_view<Components...>::each(Func func) {
	if constexpr (sizeof...(Components) == 0) {
		return;
	} else {
		using first_type = std::tuple_element_t<0, std::tuple<Components...>>;
		auto& primary = get_storage<first_type>();

		for (auto&& [entt, comp] : primary) {
			if (owner_.valid(entt) && (owner_.template has<Components>(entt) && ...)) {
				if constexpr (std::is_invocable_v<Func, entity, Components&...>) {
					func(entt, get_storage<Components>().get(entt)...);
				} else {
					func(entt);
				}
			}
		}
	}
}

template <typename... Components>
template <typename Func>
void basic_view<Components...>::each(Func func) const {
	if constexpr (sizeof...(Components) == 0) {
		return;
	} else {
		using first_type = std::tuple_element_t<0, std::tuple<Components...>>;
		const auto& primary = get_storage<first_type>();

		for (auto&& [entt, comp] : primary) {
			if (owner_.valid(entt) && (owner_.template has<Components>(entt) && ...)) {
				if constexpr (std::is_invocable_v<Func, entity, const Components&...>) {
					func(entt, get_storage<Components>().get(entt)...);
				} else {
					func(entt);
				}
			}
		}
	}
}

template <typename... Components>
std::size_t basic_view<Components...>::size() const {
	if constexpr (sizeof...(Components) == 0) {
		return 0;
	} else {
		using first_type = std::tuple_element_t<0, std::tuple<Components...>>;
		const auto& primary = get_storage<first_type>();
		std::size_t count = 0;

		for (auto&& [entt, comp] : primary) {
			if (owner_.valid(entt) && (owner_.template has<Components>(entt) && ...)) {
				++count;
			}
		}
		return count;
	}
}

template <typename... Components>
bool basic_view<Components...>::empty() const {
	return size() == 0;
}

// 组实现
template <typename... Components>
basic_group<Components...>::basic_group(registry& reg) :
		owner_(reg) {}

template <typename... Components>
void basic_group<Components...>::rebuild_if_needed() {
	if (dirty_) {
		entities_.clear();
		auto view = owner_.template create_view<Components...>();
		view.each([this](entity entt) {
			entities_.push_back(entt);
		});
		dirty_ = false;
	}
}

template <typename... Components>
void basic_group<Components...>::mark_dirty() {
	dirty_ = true;
}

template <typename... Components>
template <typename Func>
void basic_group<Components...>::each(Func func) {
	rebuild_if_needed();

	for (entity entt : entities_) {
		if (owner_.valid(entt) && (owner_.template has<Components>(entt) && ...)) {
			if constexpr (std::is_invocable_v<Func, entity, Components&...>) {
				func(entt, owner_.template get<Components>(entt)...);
			} else {
				func(entt);
			}
		}
	}
}

template <typename... Components>
template <typename Compare>
void basic_group<Components...>::sort(Compare comp) {
	rebuild_if_needed();
	std::sort(entities_.begin(), entities_.end(), comp);
}

template <typename... Components>
template <typename Component, typename Compare>
void basic_group<Components...>::sort_by_component(Compare comp) {
	rebuild_if_needed();
	std::sort(entities_.begin(), entities_.end(), [this, &comp](entity a, entity b) {
		return comp(owner_.template get<Component>(a), owner_.template get<Component>(b));
	});
}

template <typename... Components>
const std::vector<entity>& basic_group<Components...>::get_entities() {
	rebuild_if_needed();
	return entities_;
}

template <typename... Components>
std::size_t basic_group<Components...>::size() {
	rebuild_if_needed();
	return entities_.size();
}

template <typename... Components>
bool basic_group<Components...>::empty() {
	return size() == 0;
}


}

#endif