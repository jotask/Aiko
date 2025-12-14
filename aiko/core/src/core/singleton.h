#pragma once

namespace aiko
{
    
    template<typename T>
    class AIKO_API Singleton {
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
