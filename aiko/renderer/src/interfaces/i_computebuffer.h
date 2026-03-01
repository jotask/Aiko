#pragma once

#include <aiko_types.h>

#include "types/compute_pass.h"

namespace aiko
{
    namespace interfaces
    {
        struct IComputeBufferImpl
        {
            IComputeBufferImpl() = default;

            virtual ~IComputeBufferImpl() = default;

            virtual bool isValid() const = 0;

            virtual void create(ComputeBufferFormat format, uint32_t count, const void* data = nullptr) = 0;
            virtual void update(uint32_t start, uint32_t count, const void* data) = 0;

            virtual void destroy() = 0;

            // Optional: CPU readback for debugging (we can add later)
            // virtual bool readback(void* dst, uint32_t bytes) = 0;

        };
    }
}
