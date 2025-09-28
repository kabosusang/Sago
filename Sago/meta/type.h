#ifndef SG_META_TYPE_H
#define SG_META_TYPE_H

#include <cstddef>
#include <type_traits>
namespace meta::type {
template <typename T, typename... Ts>
constexpr size_t index_of_type_v = [] {
	size_t index = 0;
	bool found = ((std::is_same_v<T, Ts> ? true : (++index, false)) || ...);
	return found ? index : static_cast<size_t>(-1);
}();






} //namespace meta::type

#endif