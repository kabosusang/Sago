#ifndef SG_SINGLE_INTERNAL_H
#define SG_SINGLE_INTERNAL_H

#include <type_traits>

namespace Common {

template <typename T>
class SingletonBase {
protected:
    SingletonBase() = default;
};


template <typename T>
requires std::is_class_v<T>
class Singleton : public SingletonBase<T>{
public:
    inline static T& Instance(){
        static T instance;
        return instance;
    }
    Singleton(Singleton&&) = delete;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

protected:
    Singleton() = default;
};





}








#endif