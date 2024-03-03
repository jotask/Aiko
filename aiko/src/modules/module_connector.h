#pragma once

#include <memory>
#include <vector>

#include "aiko_types.h"
#include <modules/module.h>

namespace aiko
{
    
    class ModuleConnector
    {
    public:
    
        using Modules = std::vector<AikoUPtr<Module>>;
    
        ModuleConnector(Modules& modules)
            : m_modules(modules)
        {
        };
        ~ModuleConnector() = default;
    
        template<class T>
        T* find()
        {
            auto it = std::find_if(m_modules.begin(), m_modules.end(), [](const aiko::AikoUPtr<Module>& module) {
                return dynamic_cast<T*>(module.get()) != nullptr;
            });
            return (it != m_modules.end()) ? dynamic_cast<T*>(it->get()) : nullptr;
        }
    
    private:
        Modules& m_modules;
    };

}
