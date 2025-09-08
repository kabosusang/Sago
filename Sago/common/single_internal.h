#ifndef SG_SINGLE_INTERNAL_H
#define SG_SINGLE_INTERNAL_H

#include <type_traits>

namespace Common {

namespace Single {
struct Meyers {};
struct Global {};
}; //namespace Single

using DefaultSingetonTag = Single::Meyers;
using GlobalSingetonTag = Single::Global;

template <typename T>
class SingletonBase {
protected:
	SingletonBase() = default;
};

template <typename T, class Policy = DefaultSingetonTag>
class Singleton;

//Meyers
template <typename T>
	requires std::is_class_v<T>
class Singleton<T, Single::Meyers> : public SingletonBase<T> {
public:
	inline static T& Instance() {
		static T instance;
		return instance;
	}
	Singleton(Singleton&&) = delete;
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;

protected:
	Singleton() = default;
};

//Global

template <typename T>
	requires std::is_class_v<T>
class Singleton<T, Single::Global> : public SingletonBase<T> {
public:
	Singleton(Singleton&&) = delete;
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;

protected:
	Singleton() = default;
};

} //namespace Common

#define DEFINE_CLASS_SINGLTEN(ClassName)  \
public:                                   \
	inline static ClassName& Instance() { \
		return instance_;                 \
	}                                     \
	static ClassName instance_;

#endif