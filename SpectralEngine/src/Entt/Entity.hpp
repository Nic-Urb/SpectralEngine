//
//  EnttObject.hpp
//  SpectralEngine
//
//  Created by Nicolas U on 01.06.24.
//
#pragma once

#include "pch.h"

#include "entt.hpp"

#include "Core/Scene.hpp"
#include "Core/Assert.hpp"

#include "Components.hpp"

namespace Spectral {

    class Entity
    {
    public:
        Entity() = default;
        Entity(entt::entity handle, Scene* context);
        
        template<typename T, typename... Args>
        T& AddComponent(Args&&... args)
        {
            SP_ASSERT(!HasComponent<T>(), "Entity already has this Component");
            
            T& component = m_SceneContext->m_Registry.emplace<T>(m_EnttHandle, std::forward<Args>(args)...);
            return component;
        }
        
        template <typename T>
        T& GetComponent()
        {
            SP_ASSERT(HasComponent<T>(), "Entity does not have this component");
            
            return m_SceneContext->m_Registry.get<T>(m_EnttHandle);
        }
        
        template <typename T>
        T& GetOrAddComponent()
        {
            if (HasComponent<T>()) {
                // return this component
                return m_SceneContext->m_Registry.get<T>(m_EnttHandle);
            }
            
            return AddComponent<T>();
        }
        
        template<typename T>
        bool HasComponent()
        {
            return m_SceneContext->m_Registry.all_of<T>(m_EnttHandle);
        }
        
        template <typename T>
        bool RemoveComponent()
        {
            if (HasComponent<T>()) {
                m_SceneContext->m_Registry.remove<T>(m_EnttHandle);
                return true;
            }
            return false;
        }

        operator entt::entity() const { return m_EnttHandle; }
        operator bool() const { return m_EnttHandle != entt::null; }
        bool operator==(const Entity& other) const
        {
            return m_EnttHandle == other.m_EnttHandle && m_SceneContext == other.m_SceneContext;
        }
        bool operator!=(const Entity& other) const
        {
            return !(*this == other);
        }
        
    private:
        entt::entity m_EnttHandle{ entt::null };
        Scene* m_SceneContext = nullptr;
    };
}
