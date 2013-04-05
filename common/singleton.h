#ifndef COMMON_SINGLETON_H
#define COMMON_SINGLETON_H

#include "common/pch.h"

template<typename T>
class Singleton {
public:
    static inline std::shared_ptr<T> GetInstancePtr() {
        static std::shared_ptr<T> instance(new T);
        return instance;
    }

    static inline T& GetInstance() {
        return *GetInstancePtr();
    }

    Singleton() {}

protected:
    Singleton(const Singleton<T> &);
    Singleton& operator=(const Singleton<T> &);
};

#endif // COMMON_SINGLETON_H
