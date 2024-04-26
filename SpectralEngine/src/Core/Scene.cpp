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

    void Scene::OnUpdateRuntime(Timestep ts)
    {
        for (const auto& [_, v] : m_ObjectRegistry)
        {
            // find objects with camera component
            CameraComponent* cameraComponent = v->GetComponent<CameraComponent>();
            if (cameraComponent)
            {
                // find active runtime camera
                if (cameraComponent->IsActive()) {
                    m_RuntimeCamera = cameraComponent->GetCamera();
                    break;
                } else {
                    m_RuntimeCamera = nullptr;
                }
            }
            
            v->OnUpdate(ts); // update each components
        }
    }

    void Scene::OnRenderRuntime()
    {
        if (m_RuntimeCamera)
        {
            BeginMode3D(m_RuntimeCamera->GetCamera3D());
            
                // draw sprites
                for (const auto& [_, v] : m_ObjectRegistry)
                {
                    SpriteComponent* spriteComponent = v->GetComponent<SpriteComponent>();
                    if (spriteComponent)
                    {
                        spriteComponent->OnRender(m_RuntimeCamera->GetCamera3D());
                    }
                }
            
                // @TODO: draw text, draw 3d models, ...
                
            EndMode3D();
        }
    }

    void Scene::OnRenderEditor(EditorCamera& camera)
    {
        BeginMode3D(camera.GetCamera3D());
            
            // draw grid
            DrawGrid(20, 10.0f);
        
            // draw sprites
            for (const auto& [_, v] : m_ObjectRegistry)
            {
                SpriteComponent* spriteComponent = v->GetComponent<SpriteComponent>();
                if (spriteComponent)
                {
                    spriteComponent->OnRender(camera.GetCamera3D());
                }
            }
        
        // @TODO: draw text, draw 3d models, ...
        
        EndMode3D();
    }

}
