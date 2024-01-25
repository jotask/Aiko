#pragma once

#include "base_module.h"

class CameraModule : public BaseModule
{
public:

    CameraModule() = default;
    virtual ~CameraModule() = default;

protected:

    virtual void init() override;

};