#pragma once

#include <aiko_types.h>

namespace aiko
{
    namespace interfaces
    {
        struct IComputeShaderImpl
        {
            IComputeShaderImpl() = default;

            virtual ~IComputeShaderImpl() = default;

            virtual bool isValid() const = 0;

            // load
            virtual void load(string file) = 0;
            virtual void unload() = 0;

            virtual uint id() = 0;

        };
    }
}
