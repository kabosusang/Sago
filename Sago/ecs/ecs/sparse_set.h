#ifndef SG_ECS_SPARSESET_H
#define SG_ECS_SPARSESET_H
#include "entity.h"

#include <memory>
#include <vector>

namespace SECS {
template <typename Entity, typename Allocator = std::allocator<Entity>>
class basic_sparse_set {
	std::vector<Entity, Allocator> dense;
	std::vector<entity> sparse;

public:
	using iterator = typename std::vector<Entity>::iterator;
	using const_iterator = typename std::vector<Entity>::const_iterator;
	basic_sparse_set() = default;

	bool contains(const Entity entt) const noexcept {
		const auto idx = static_cast<std::size_t>(entity_id(entt));
		return idx < sparse.size() && sparse[idx] != tombstone &&
				sparse[idx] < dense.size() && dense[sparse[idx]] == entt;
	}

	std::size_t index(const Entity entt) const noexcept {
		return contains(entt) ? sparse[static_cast<std::size_t>(entity_id(entt))] : dense.size();
	}

	void emplace(const Entity entt) {
		const auto idx = static_cast<std::size_t>(entity_id(entt));

		if (idx >= sparse.size()) {
			sparse.resize(idx + 1, tombstone);
		}

		if (!contains(entt)) {
			sparse[idx] = static_cast<entity>(dense.size());
			dense.push_back(entt);
		}
	}

	void erase(const Entity entt) {
		if (contains(entt)) {
			const auto idx = static_cast<std::size_t>(entity_id(entt));
			const auto pos = sparse[idx];
			const auto back = dense.back();

			if (pos != dense.size() - 1) {
				std::swap(dense[pos], dense.back());
				sparse[static_cast<std::size_t>(entity_id(back))] = pos;
			}

			dense.pop_back();
			sparse[idx] = tombstone;
		}
	}

	void clear() noexcept {
		dense.clear();
		std::fill(sparse.begin(), sparse.end(), tombstone);
	}

	std::size_t size() const noexcept { return dense.size(); }
	bool empty() const noexcept { return dense.empty(); }

	const_iterator begin() const noexcept { return dense.begin(); }
	const_iterator end() const noexcept { return dense.end(); }
	iterator begin() noexcept { return dense.begin(); }
	iterator end() noexcept { return dense.end(); }

	const Entity* data() const noexcept { return dense.data(); }
	Entity* data() noexcept { return dense.data(); }

	const std::vector<Entity, Allocator>& get_dense() const noexcept { return dense; }
	std::vector<Entity, Allocator>& get_dense() noexcept { return dense; }
};


template <typename Type, typename Entity = entity>
class basic_storage {
private:
	basic_sparse_set<Entity> sparse_set_;
	std::vector<Type> components_;

public:
	using value_type = Type;
	using entity_type = Entity;

	bool contains(const Entity entt) const noexcept {
		return sparse_set_.contains(entt);
	}

	Type& get(const Entity entt) {
		return components_[sparse_set_.index(entt)];
	}

	const Type& get(const Entity entt) const {
		return components_[sparse_set_.index(entt)];
	}

	template <typename... Args>
	Type& emplace(const Entity entt, Args&&... args) {
		sparse_set_.emplace(entt);
		const std::size_t idx = sparse_set_.index(entt);

		if (idx >= components_.size()) {
			components_.emplace_back(std::forward<Args>(args)...);
		} else {
			components_[idx] = Type{ std::forward<Args>(args)... };
		}
		return get(entt);
	}

	template <typename Component>
	Type& emplace(const Entity entt, Component&& component) {
		sparse_set_.emplace(entt);
		const std::size_t idx = sparse_set_.index(entt);

		if (idx >= components_.size()) {
			components_.emplace_back(std::forward<Component>(component));
		} else {
			components_[idx] = Type{ std::forward<Component>(component) };
		}
		return get(entt);
	}

	void erase(const Entity entt) {
		const auto pos = sparse_set_.index(entt);
		if (pos < sparse_set_.size()) {
			if (pos != sparse_set_.size() - 1) {
				std::swap(components_[pos], components_.back());
			}
			components_.pop_back();
		}
		sparse_set_.erase(entt);
	}

	struct iterator {
		typename basic_sparse_set<Entity>::iterator entity_it;
		Type* comp_ptr;

		auto operator*() const -> std::pair<Entity, Type&> {
			return { *entity_it, *comp_ptr };
		}
		iterator& operator++() {
			++entity_it;
			++comp_ptr;
			return *this;
		}
		bool operator!=(const iterator& other) const {
			return entity_it != other.entity_it;
		}
	};

	struct const_iterator {
		typename basic_sparse_set<Entity>::const_iterator entity_it;
		const Type* comp_ptr;

		auto operator*() const -> std::pair<Entity, const Type&> {
			return { *entity_it, *comp_ptr };
		}
		const_iterator& operator++() {
			++entity_it;
			++comp_ptr;
			return *this;
		}
		bool operator!=(const const_iterator& other) const {
			return entity_it != other.entity_it;
		}
	};

	iterator begin() noexcept {
		return iterator{ sparse_set_.begin(), components_.data() };
	}

	iterator end() noexcept {
		return iterator{ sparse_set_.end(), components_.data() + sparse_set_.size() };
	}

	const_iterator begin() const noexcept {
		return const_iterator{ sparse_set_.begin(), components_.data() };
	}

	const_iterator end() const noexcept {
		return const_iterator{ sparse_set_.end(), components_.data() + sparse_set_.size() };
	}

	std::size_t size() const noexcept { return sparse_set_.size(); }
	bool empty() const noexcept { return sparse_set_.empty(); }

	const basic_sparse_set<Entity>& get_sparse_set() const noexcept { return sparse_set_; }
};



} //namespace SECS
#endif