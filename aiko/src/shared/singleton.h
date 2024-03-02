#pragma once

#include "aiko_types.h"

namespace aiko
{
    
    template<typename T>
    class Singleton {
    public:
        static T& it() {
            static T instance;
            return instance;
        }

    protected:
        Singleton() = default;
        virtual ~Singleton() = default;
        Singleton(const Singleton&) = delete;
        Singleton& operator=(const Singleton&) = delete;
    };

}
