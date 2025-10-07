#ifndef SG_CORE_HASH_FUNCTION_h
#define SG_CORE_HASH_FUNCTION_h
#include <cstddef>

namespace Core {

constexpr std::size_t hash_fnv1a(const char* str, std::size_t hash = 14695981039346656037ULL) {
	return (*str == '\0') ? hash : hash_fnv1a(str + 1, (hash ^ static_cast<std::size_t>(*str)) * 1099511628211ULL);
}

} //namespace Core

#endif