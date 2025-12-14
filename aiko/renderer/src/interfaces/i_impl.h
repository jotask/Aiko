#pragma once

namespace aiko
{
    struct ImplBase
    {
        virtual void* nativeHandle() const = 0;
    };

}