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


namespace Spectral {

    class Object; // fwd declaration

    class Scene
    {
    public:
        Scene() {}
        ~Scene() {}
        
        template <typename T, typename... Args>
        T* CreateObject(Args... args)
        {
            // ensure on compile-time that we only try to add a class derives from Object
            static_assert(std::is_base_of_v<Object, T>, "T must derive from Object");
            
            std::unique_ptr<T> object = std::make_unique<T>(args...);
            object->SetUUID(UUID()); // Calls UUID() constructor which create's unique id
            UUID uuid = object->GetUUID();
            m_ObjectRegistry.emplace(uuid, std::move(object));
            
            
            return static_cast<T*>(m_ObjectRegistry[uuid].get());
        }
        
        bool RemoveObject(UUID uuid);
        
        // OnRuntimeStart(...)
        // OnRuntimePause()
        // OnRuntimeEnd()
        
        //void OnUpdateRuntime(Timestep ts, EditorCamera& camera);
        //void OnUpdateRuntime(EditorCamera& camera);
        
        void OnUpdateEditor(Timestep ts, EditorCamera& camera/* Do we need camera ?? */);
        void OnRenderEditor(EditorCamera& camera); // this update should be called from client layer update function
        
    private:
        std::unordered_map<UUID, std::unique_ptr<Object>> m_ObjectRegistry;
        
        
        friend class HierarchyPanel; // allow access to private members

    };
}
