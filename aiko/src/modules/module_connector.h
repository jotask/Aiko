#pragma once

#include <memory>
#include <vector>

#include <modules/module.h>

namespace aiko
{
    
    class ModuleConnector
    {
    public:
    
        using Modules = std::vector<std::shared_ptr<Module>>;
    
        ModuleConnector(Modules& modules)
            : m_modules(modules)
        {
        };
        ~ModuleConnector() = default;
    
        template<class T>
        std::shared_ptr<T> find()
        {
            auto it = std::find_if(m_modules.begin(), m_modules.end(), [](const std::shared_ptr<Module>& module) {
                return dynamic_cast<T*>(module.get()) != nullptr;
            });
            return (it != m_modules.end()) ? std::dynamic_pointer_cast<T>(*it) : nullptr;
        }
    
    private:
        Modules& m_modules;
    };

}
