//
//  Object.cpp
//  SpectralEngine
//
//  Created by Nicolas U on 30.03.24.
//

#include "Object.hpp"

namespace Spectral {

    Object::~Object()
    {
        // allocating memory
        m_Components.clear(); // smart pointers should handle deletion automatically
        delete m_Owner;
        SP_LOG_ERORR("Object::Allocated memory");
    }

    bool Object::HasComponent(uint32_t id)
    {
        auto it = m_Components.find(id);
        
        if (it != m_Components.end()) {
            return true;
        }
        return false;
    }

    bool Object::RemoveComponent(uint32_t id)
    {
        auto it = m_Components.find(id);
        
        if (it != m_Components.end())
        {
            it->second->OnDestroy();
            m_Components.erase(it);
            return true;
        }
        return false;
    }

    void Object::SetOwner(Object *owner)
    {
        if (owner != nullptr)
        {
            m_Owner = owner;
        }
    }

    void Object::SetName(const std::string& name)
    {
        m_Name = name;
    }

    void Object::OnUpdate(Timestep ts)
    {
        for (const auto& [_, v] : m_Components)
        {
            if (v)
            {
                v->OnUpdate(ts);
            }
        }
    }


// ---- Static Object

    StaticObject::StaticObject() : Object()
    {
        m_Transform = AddComponent<TransformComponent>();
        m_Sprite = AddComponent<SpriteComponent>();
    }
    
}
