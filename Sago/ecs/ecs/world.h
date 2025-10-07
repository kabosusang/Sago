// ecs/world.hpp
#pragma once
#include "registry.h"
#include <functional>
#include <iostream>
#include <string>
#include <string_view>

namespace SECS {
template<typename... Components>
class SystemBuilder;

template<>
class SystemBuilder<> {
    std::function<void()> func_;

public:
    SystemBuilder(registry&) {} 
    
    SystemBuilder& each(std::function<void()> func) {
        func_ = func;
        return *this;
    }

    void operator()() {
        if (func_) func_();
    }
};

template<typename... Components>
class SystemBuilder {
    registry& registry_;
    std::function<void(Components&...)> func_;

public:
    SystemBuilder(registry& reg) : registry_(reg) {}

    SystemBuilder& each(std::function<void(Components&...)> func) {
        func_ = func;
        return *this;
    }

    void operator()() {
        if (!func_) return;
        auto view = registry_.template create_view<Components...>();
        view.each([this](entity e, Components&... comps) {
            func_(comps...);
        });
    }
};

class world {
    registry registry_;
    std::vector<std::function<void()>> systems_;

public:

    template<typename... Components>
    auto system() {
        if constexpr (sizeof...(Components) == 0) {
            return SystemBuilder<>(registry_);
        } else {
            return SystemBuilder<Components...>(registry_);
        }
    }

    void add_system(auto system) {
        systems_.push_back(system);
    }

    bool progress() {
        for (auto& system : systems_) {
            system();
        }
        return !systems_.empty();
    }

    auto entity() {
        return registry_.create();
    }

    registry& registry() { return registry_; }
    
    auto entity(std::string_view);
    
    // template<typename Component>
    // auto& get(){
    //     registry_.get<>(entity entt)
    // }
};


class Entity {
    registry* registry_;
    entity id_;
    
public:
    Entity(registry& res, entity entt) : registry_(&res), id_(entt) {}
    

    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;  

    template<typename Component, typename... Args>
    Entity& set(Args&&... args) {
        registry_->emplace<Component>(id_, std::forward<Args>(args)...);
        return *this;
    }

    template<typename Component>
    Entity& set(Component&& component) {
        registry_->emplace<Component>(id_, std::forward<Component>(component));
        return *this;
    }

    template<typename Component>
    Component& get() {
        return registry_->get<Component>(id_);
    }
    
    template<typename Component>
    const Component& get() const {
        return registry_->get<Component>(id_);
    }
    
    template<typename Component>
    bool has() const {
        return registry_->has<Component>(id_);
    }
    
    template<typename Component>
    void remove() {
        registry_->remove<Component>(id_);
    }
    
    entity id() const { return id_; }
    bool valid() const { return registry_->valid(id_); }
};

inline auto world::entity(std::string_view){
    return Entity{registry_, registry_.create()};
}







} // namespace SECS