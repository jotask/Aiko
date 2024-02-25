#include "singleton.h"

#include <memory>

namespace aiko
{
    
    template<typename T>
    T& Singleton<T>::it()
    {
        static const std::unique_ptr<T> instance{ new T{token{}} };
        return *instance;
    }

}
