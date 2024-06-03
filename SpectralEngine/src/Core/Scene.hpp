//
//  Scene.hpp
//  SpectralEngine
//
//  Created by Nicolas U on 05.04.24.
//
#pragma once

#include "pch.h"

#include "Core/UUID.hpp"
#include "Renderer/EditorCamera.hpp"
#include "Renderer/RuntimeCamera.hpp"

#include "entt.hpp"

namespace Spectral {

    class Entity; // fwd declaration

    class Scene
    {
    public:
        Scene(const std::string& name) : m_Name(name) {}
        ~Scene() {}
        
        Entity CreateEntity(UUID uuid, const std::string& name);
        void RemoveEntity(Entity entity);
        
        void OnRuntimeStart(); // @TODO: Call this
        void OnRuntimeEnd(); // @TODO: Call this
        // OnRuntimePause() @TODO: Implement this
        
        void OnUpdateRuntime(Timestep ts);
        void OnRenderRuntime();
        
        void OnUpdateEditor(Timestep ts);
        void OnRenderEditor(EditorCamera& camera); // this update should be called from client layer update function
        
        const size_t GetEntityCount() const { return m_EntityMap.size(); }
        const std::string& GetName() { return m_Name; }
        
    private:
        entt::registry m_Registry;
        
        std::unordered_map<UUID, entt::entity> m_EntityMap;
        std::shared_ptr<RuntimeCamera> m_RuntimeCamera;
        std::string m_Name;
        
        // allow access to private members
        friend class Entity;
        friend class SceneSerializer;
        friend class HierarchyPanel;
        friend class EditorLayer;
    };
}
