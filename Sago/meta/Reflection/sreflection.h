#ifndef SG_META_STATIC_REFLECTION_H
#define SG_META_STATIC_REFLECTION_H

#include <string_view>
#include <type_traits>
#include <tuple>

// Class Function
template <typename Func>
struct BaseFunction;

template <typename Ret, typename... Args>
struct BaseFunction<Ret (*)(Args...)> {
	using args = std::tuple<Args...>;
	using return_type = Ret;
};

template <typename Ret, typename... Args>
struct BaseFunction<Ret(Args...)> {
	using args = std::tuple<Args...>;
	using return_type = Ret;
};

// Class Variable 
template <typename T>
struct BaseVariable;

template <typename Class, typename T>
struct BaseVariable<T Class::*> {
	using type = T;
};
template <typename Class, typename T>
struct BaseVariable<const T Class::*> {
	using type = const T;
};

//Function Traits
template <typename Func>
struct FunctionTraits;

template <typename Ret, typename Class, typename... Args>
struct FunctionTraits<Ret (Class::*)(Args...)> 
: public BaseFunction<Ret(Class::*)(Args...)> {
    using type = Ret(Class::*)(Args...);
};

template <typename Ret, typename Class, typename... Args>
struct FunctionTraits<Ret (Class::*)(Args...) const> 
: public BaseFunction<Ret(Class::*)(Args...)> {
    using type = Ret(Class::*)(Args...) const;
};

//Variable Traits
template <typename Func>
struct VariableTraits;

template <typename Class,typename T>
struct VariableTraits<T Class::*>:public BaseVariable<T Class::*>{
	using pointer_type = T Class::*;
	using clazz = Class;
};

template <typename Class,typename T>
struct VariableTraits<const T Class::*>:public BaseVariable<const T Class::*>{
	using pointer_type = const T Class::*;
	using clazz = Class;
};

template <typename T, bool isFunc>
struct BaseField;

template <typename T>
struct BaseField<T, true>: public FunctionTraits<T>{
   
};

template <typename T>
struct BaseField<T, false>: public BaseVariable<T>{
    static constexpr size_t size = sizeof(T);
    static constexpr size_t align = alignof(T);
};

template <typename T>
struct Field : public BaseField<T, std::is_fundamental_v<T>> {
	consteval Field(T&& pointer,std::string_view name):pointer{ pointer },name_(name = name.substr(name.find_last_of(":") + 1)){}
	T pointer;
    std::string_view name_;
};

template <typename T>
struct TypeInfo;

template <typename T>
consteval auto type_info_helper() {
	return TypeInfo<T>{};
}

#define BEGIN_CLASS(X) template <> \
struct TypeInfo<X> {                \
friend X;

#define functions(...) static constexpr auto functions = std::make_tuple(__VA_ARGS__);
#define func(F)    \
	Field { \
		F,#F      \
	}

#define variables(...) static constexpr auto variables = std::make_tuple(__VA_ARGS__);
#define var(V) Field{&V, #V}

#define END_CLASS() \
	}               \
	;
















#endif
