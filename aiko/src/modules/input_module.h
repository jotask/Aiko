#pragma once

#include "base_module.h"

class InputModule : public BaseModule
{
public:

    InputModule() = default;
    virtual ~InputModule() = default;

protected:

    virtual void preUpdate() override;

};