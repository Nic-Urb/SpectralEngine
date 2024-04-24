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
        
        // OnRuntimeStart(...) @TODO: Implement this
        // OnRuntimePause() @TODO: Implement this
        // OnRuntimeEnd() @TODO: Implement this
        
        void OnUpdateRuntime(Timestep ts);
        void OnRenderRuntime();
        
        void OnRenderEditor(EditorCamera& camera); // this update should be called from client layer update function
        
    private:
        std::unordered_map<UUID, std::unique_ptr<Object>> m_ObjectRegistry;
        std::shared_ptr<RuntimeCamera> m_RuntimeCamera;
        
        friend class HierarchyPanel; // allow access to private members
    };
}
