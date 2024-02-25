#pragma once

#include "aiko_types.h"

namespace aiko
{
    
    template<typename T>
    class Singleton {
    public:
        static T& it();

        Singleton(const Singleton&) = delete;
        Singleton& operator= (const Singleton) = delete;

    protected:
        struct token {};
        Singleton() {}
    };


}
