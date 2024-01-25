#pragma once

#include <vector>
#include <stdexcept>
#include <string>

#include "aiko.h"
#include "aiko_types.h"
#include "component.h"
#include "components/transform_component.h"

class Aiko;

class GameObject
{
    friend class EntityComponentSystem;
    friend class Component; // FIXME, so we can easily create components
public:

    GameObject() = default;
    ~GameObject() = default;

    template<class T>
    aiko::AikoPtr<T> addComponent();

    template<class T>
    bool hasComponent();

    template<class T>
    aiko::AikoPtr<T> getComponent();

    template<class T>
    bool removeComponent();

    std::string getName() const { return name; }
    void setName( std::string newName ) { name = newName; }

    aiko::AikoPtr<Transform> transform();

    // FIXME: For now, so we can esily create components
    template<class T>
    auto getSystem();

private:

    // FIXME: For now, so we can esily create components
    Aiko* aiko;
    void setup(Aiko* aiko) { this->aiko = aiko; };

    std::string name;
    std::vector<aiko::AikoPtr<Component>> m_components;

    void update();
    void render();

};

template<class T>
inline aiko::AikoPtr<T> GameObject::addComponent()
{
    if (hasComponent<T>() == true)
    {
        // throw std::exception();
    }
    m_components.emplace_back(std::make_shared<T>());
    aiko::AikoPtr<Component> back = m_components.back();
    back->setup(this);
    back->init();
    return std::dynamic_pointer_cast<T>(back);
}

template<class T>
bool GameObject::hasComponent()
{
    auto it = std::find_if(m_components.begin(), m_components.end(), [](const aiko::AikoPtr<Component>& component) {
        return dynamic_cast<T*>(component.get()) != nullptr;
    });
    return it != m_components.end();
}

template<class T>
inline aiko::AikoPtr<T> GameObject::getComponent()
{
    if (hasComponent<T>() == false)
    {
        throw std::exception();
    }
    auto it = std::find_if(m_components.begin(), m_components.end(), [](const std::shared_ptr<Component>& component) {
        return dynamic_cast<T*>(component.get()) != nullptr;
    });
    return (it != m_components.end()) ? std::dynamic_pointer_cast<T>(*it) : nullptr;
}

template<class T>
bool GameObject::removeComponent()
{
    if (hasComponent<T>() == false)
    {
        throw std::exception();
    }
    return m_components.erase(std::remove(m_components.begin(), m_components.end(), [](const std::shared_ptr<Component>& component) {
        return dynamic_cast<T*>(component.get()) != nullptr;
    }), m_components.end());
}

template<class T>
inline auto GameObject::getSystem()
{
    return aiko->getSystem<T>();
}
