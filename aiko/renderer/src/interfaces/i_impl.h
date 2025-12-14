#pragma once

#include <aiko_types.h>

namespace aiko
{
    struct AIKO_API ImplBase
    {
        virtual void* nativeHandle() const = 0;
    };

}