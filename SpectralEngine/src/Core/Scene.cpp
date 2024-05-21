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
            it->second->OnDestroy();
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
                if (cameraComponent->Active) {
                    m_RuntimeCamera = cameraComponent->Camera;
                } else {
                    m_RuntimeCamera = nullptr; // @TODO: Investigate this 
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
                        spriteComponent->OnRender();
                    }
                }
            
                // @TODO: draw text, draw 3d models, ...
                
            EndMode3D();
        }
    }

    void Scene::OnUpdateEditor(Timestep ts)
    {
        for (const auto& [_, v] : m_ObjectRegistry)
        {
            // update sprites
            SpriteComponent* spriteComponent = v->GetComponent<SpriteComponent>();
            if (spriteComponent)
            {
                spriteComponent->OnUpdate(ts);
            }
        }
    }

    void Scene::OnRenderEditor(EditorCamera& camera, const RayCollision& collisionInfo)
    {
        BeginMode3D(camera.GetCamera3D());
            
            // draw grid
            DrawGrid(20, 10.0f);
        
            for (const auto& [_, v] : m_ObjectRegistry)
            {
                // draw sprites
                SpriteComponent* spriteComponent = v->GetComponent<SpriteComponent>();
                if (spriteComponent)
                {
                    spriteComponent->OnRender();
                    
                    // @TODO: DEBUG ONLY, delete this later
                    DrawLine3D(spriteComponent->Bounds[0], spriteComponent->Bounds[1], VIOLET);
                    DrawLine3D(spriteComponent->Bounds[1], spriteComponent->Bounds[2], VIOLET);
                    DrawLine3D(spriteComponent->Bounds[2], spriteComponent->Bounds[3], VIOLET);
                    DrawLine3D(spriteComponent->Bounds[3], spriteComponent->Bounds[0], VIOLET);
                }
                
                // draw camera debug
                CameraComponent* cameraComponent = v->GetComponent<CameraComponent>();
                if (cameraComponent) {
                    if (cameraComponent->Debug) {
                        cameraComponent->OnRender();
                    }
                }
            }
        
        // @TODO: draw text, draw 3d models, ...
        
        
        // Draw from collision info (Editor only)
        // @TODO: DEBUG ONLY, delete this later
        if (collisionInfo.hit) {
            DrawCube(collisionInfo.point, 3.0f, 3.0f, 3.0f, BLUE);
            DrawCubeWires(collisionInfo.point, 3.0f, 3.0f, 3.0f, RED);
        }
        
        
        EndMode3D();
    }
}
