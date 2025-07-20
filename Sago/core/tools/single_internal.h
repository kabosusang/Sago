#ifndef SG_SINGLE_INTERNAL_H
#define SG_SINGLE_INTERNAL_H


namespace Tools {

template <typename T>
class Singleton{
public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    
    static T& Instance(){
        static T instance;
        return instance;
    }
protected:
    Singleton() = default;
    virtual ~Singleton() = default;
};





}








#endif