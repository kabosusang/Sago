#ifndef SG_ECS_VIEW_H
#define SG_ECS_VIEW_H
#include <cstddef>

namespace SECS{
class registry;

template <typename... Components>
class basic_view {
private:
    registry& owner_;

    template<typename Comp>
    auto& get_storage();

    template<typename Comp>
    const auto& get_storage() const;

public:
    basic_view(registry& reg);
    
    template<typename Func>
    void each(Func func);
    
    template<typename Func>
    void each(Func func) const;
    
    std::size_t size() const;
    bool empty() const;
};



}





#endif