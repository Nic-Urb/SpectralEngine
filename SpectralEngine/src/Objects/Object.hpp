//
//  Object.hpp
//  SpectralEngine
//
//  Created by Nicolas U on 30.03.24.
//
#pragma once

#include "pch.h"

#include "raylib.h"
#include "Component.hpp"
#include "Core/Timestep.h"
#include "Core/UUID.hpp"


namespace Spectral {

    // Should provides the fundamental mechanisms for object lifecycle management, and serialization.
    class Object
    {
    public:
        ~Object();
        
        template <typename T>
        T* AddComponent()
        {
            // ensure on compile-time that we only try to add a class derives from Component
            static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
            
            std::unique_ptr<T> component = std::make_unique<T>(*this);
            component->OnConstruct();
            uint32_t id = T::GetComponentId();
            m_Components.emplace(id, std::move(component));
            
            SP_LOG_INFO("Adding component ({0})", T::GetComponentName());
            
            return static_cast<T*>(m_Components[id].get());
        }
        
        template <typename T>
        T* GetComponent()
        {
            // ensure on compile-time that we only try to add a class derives from Component
            static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
            
            auto it = m_Components.find(T::GetComponentId());
            
            if (it != m_Components.end())
            {
                return static_cast<T*>(it->second.get());
            }
            return nullptr;
        }
        
        bool HasComponent(uint32_t id);
        bool RemoveComponent(uint32_t id);
        
        void SetOwner(Object* owner);
        Object* GetOwner() const { return m_Owner; }
        
        void SetName(const std::string& name);
        const std::string& GetName() const { return m_Name; }
        
        void SetUUID(UUID id) { m_UUID = id; }
        UUID GetUUID() const { return m_UUID; }
        
        virtual void OnUpdate(Timestep ts); // optional
    
    protected:
        std::unordered_map<uint32_t, std::unique_ptr<Component>> m_Components;
    
    private:
        Object* m_Owner = nullptr;
        std::string m_Name = "Object";
        UUID m_UUID;
    
    };


    class StaticObject : public Object
    {
    public:
        StaticObject();
        
        TransformComponent* GetTransformComponent() const { return m_Transform; }
        
    private:
        TransformComponent* m_Transform = nullptr;
        SpriteComponent* m_Sprite = nullptr;
        
    };


    // Objects with behaviours
    class RuntimeObject : public Object
    {
    public:
        void OnUpdate(Timestep ts) override {}
    };


    // temp - delete
    class TestingObject : public Object
    {
    public:
    };

}
