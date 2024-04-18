//
//  Scene.cpp
//  SpectralEngine
//
//  Created by Nicolas U on 05.04.24.
//

#include "Scene.hpp"

#include "Objects/Object.hpp"
#include "Objects/Component.hpp"

#include "raylib.h"

namespace Spectral {

    bool Scene::RemoveObject(UUID uuid)
    {
        auto it = m_ObjectRegistry.find(uuid);
        
        if (it != m_ObjectRegistry.end())
        {
            m_ObjectRegistry.erase(it);
            return true;
        }
        return false;
    }

    void Scene::OnUpdateEditor(Timestep ts, EditorCamera &camera)
    {
        // update components
        for (const auto& [_, v] : m_ObjectRegistry)
        {
            v->OnUpdate(ts);
        }
    }

    void Scene::OnRenderEditor(EditorCamera& camera)
    {
        BeginMode2D(camera.GetCamera());
            
            // draw sprites
            for (const auto& [_, v] : m_ObjectRegistry)
            {
                SpriteComponent* spriteComponent = v->GetComponent<SpriteComponent>();
                
                if (spriteComponent)
                {
                    spriteComponent->OnRender();
                }
            }
        
            // draw text
        
        EndMode2D();
    }

}
