#ifndef SG_ECS_GENERATOR_H
#define SG_ECS_GENERATOR_H

#include "core/util/dll_export.h"
#include "core/util/hash_func.h"
#include <cstddef>
#include <type_traits>

namespace SECS {
#define USE_HASH_FUNCTION_GENERATOR

#if defined USE_HASH_FUNCTION_GENERATOR
#if defined _MSC_VER
#define GENERATOR_PRETTY_FUNCTION __FUNCSIG__
#elif defined __clang__ || (defined __GNUC__)
#define GENERATOR_PRETTY_FUNCTION __PRETTY_FUNCTION__
#endif
#endif
struct SAGA_DEF_DLLEXPORT Generator {
	static std::size_t next() {
		static std::size_t value{};
		return ++value;
	}
};

template <typename Type, typename = void>
struct SAGA_DEF_DLLEXPORT type {
#if defined GENERATOR_PRETTY_FUNCTION
	static constexpr std::size_t id() {
		constexpr auto value = Core::hash_fnv1a(GENERATOR_PRETTY_FUNCTION);
		return value;
	}
#else
	static std::size_t id() {
		static const std::size_t value = generator::next();
		return value;
	}
#endif
};

template <typename Type>
struct SAGA_DEF_DLLEXPORT type<Type, std::void_t<decltype(Type::id())>> {
	static constexpr std::size_t id() {
		return Type::id();
	}
};

//EXAMPLE
/*
    struct Health {
        static constexpr std::size_t id() { return 9999; } 
    };
*/

} //namespace ECS

#endif