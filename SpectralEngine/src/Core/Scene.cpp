//
//  Scene.cpp
//  SpectralEngine
//
//  Created by Nicolas U on 05.04.24.
//

#include "Scene.hpp"

#include "Entt/Entity.hpp"

#include "Renderer/Renderer.hpp"

#include "raylib.h"
#include "raymath.h"

namespace Spectral {

    Entity Scene::CreateEntity(UUID uuid, const std::string& name)
    {
        Entity entity = { m_Registry.create(), this };
        entity.AddComponent<IDComponent>(uuid, name.empty() ? "Entity" : name);
        entity.AddComponent<TransformComponent>();
        
        m_EntityMap[uuid] = entity;
        return entity;
    }

    void Scene::RemoveEntity(Entity entity)
    {
        m_EntityMap.erase(entity.GetUUID());
        m_Registry.destroy(entity);
    }

    void Scene::OnRuntimeStart()
    {}

    void Scene::OnRuntimeEnd()
    {}

    void Scene::OnUpdateRuntime(Timestep ts)
    {
        // update scripts
        
        
        // camera
        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view)
        {
            Entity entt = { entity, this };
            auto& camera = entt.GetComponent<CameraComponent>();
            
            if (camera.Active) {
                m_RuntimeCamera = camera.Camera;
            } else {
                m_RuntimeCamera = nullptr;
            }
        }
    }

    void Scene::OnRenderRuntime()
    {
        if (m_RuntimeCamera)
        {
            BeginMode3D(m_RuntimeCamera->GetCamera3D());
            
                // draw sprites
                {
                    auto view = m_Registry.view<TransformComponent, SpriteComponent>();
                    for (auto entity : view)
                    {
                        auto [transform, sprite] = view.get<TransformComponent, SpriteComponent>(entity);
                        
                        transform.PushMatrix();
                        
                        Renderer::RenderTexturedPlane(sprite.SpriteTexture, sprite.Tint);
                        
                        transform.PopMatrix();
                    }
                }
            
                // draw 3D models
                {
                    auto view = m_Registry.view<TransformComponent, ModelComponent>();
                    for (auto entity : view)
                    {
                        auto [transform, model] = view.get<TransformComponent, ModelComponent>(entity);
                        
                        transform.PushMatrix();
                        
                            DrawModel(model.ModelData, (Vector3){0.0f, 2.0f, 0.0f}, 1.0f, WHITE); // @TODO: Temp
                        
                        transform.PopMatrix();
                    }
                }
            
            EndMode3D();
        }
    }

    void Scene::OnUpdateEditor(Timestep ts)
{
        // @TODO: Temp solution to get bounds for sprites
        auto view = m_Registry.view<SpriteComponent, TransformComponent>();
        for (auto entity : view)
        {
            // Update sprites bounds
            auto [transform, sprite] = view.get<TransformComponent, SpriteComponent>(entity);
            
            Vector3 position;
            Vector2 size;
            Vector3 rotation;
            
            position = transform.Translation;
            size = (Vector2){(float)sprite.SpriteTexture.width * transform.Scale.x, (float)sprite.SpriteTexture.height * transform.Scale.z};
            rotation = transform.Rotation;
            
            sprite.Bounds[0] = (Vector3){position.x - size.x / 2, position.y, position.z - size.y / 2};        // Top left corner
            sprite.Bounds[1] = (Vector3){position.x + size.x / 2, position.y, position.z - size.y / 2};        // Top right corner
            sprite.Bounds[2] = (Vector3){position.x + size.x / 2, position.y, position.z + size.y / 2};        // Bottom right corner
            sprite.Bounds[3] = (Vector3){position.x - size.x / 2, position.y, position.z + size.y / 2};        // Bottom left corner
            
            // Only calculate if we have some rotation values
            if (rotation.x != 0.0f || rotation.y != 0.0f || rotation.z != 0.0f)
            {
                Matrix rotX = MatrixRotateX(rotation.x * DEG2RAD);
                Matrix rotY = MatrixRotateY(rotation.y * DEG2RAD);
                Matrix rotZ = MatrixRotateZ(rotation.z * DEG2RAD);
                
                // Combine the rotation matrices (rotZ * (rotY * rotX))
                Matrix rotationMatrix = MatrixMultiply(rotZ, MatrixMultiply(rotY, rotX));
                
                // Apply the combined rotation matrix to each corner
                for (int i = 0; i < 4; ++i)
                {
                    Vector3 corner = (Vector3){sprite.Bounds[i].x - position.x, sprite.Bounds[i].y - position.y, sprite.Bounds[i].z - position.z};
                    Vector3 transformed = Vector3Transform(corner, rotationMatrix);
                    sprite.Bounds[i] = (Vector3){transformed.x + position.x, transformed.y + position.y, transformed.z + position.z};
                }
            }
        }
        
    }

    void Scene::OnRenderEditor(EditorCamera& camera)
    {
        BeginMode3D(camera.GetCamera3D());
            
            // draw grid
            DrawGrid(20, 10.0f);
        
            // draw sprites
            {
                auto view = m_Registry.view<TransformComponent, SpriteComponent>();
                for (auto entity : view)
                {
                    auto [transform, sprite] = view.get<TransformComponent, SpriteComponent>(entity);
                    
                    transform.PushMatrix();
                    
                        Renderer::RenderTexturedPlane(sprite.SpriteTexture, sprite.Tint);
                    
                    transform.PopMatrix();
                    
                    // @TODO: DEBUG ONLY, delete this later
                    DrawLine3D(sprite.Bounds[0], sprite.Bounds[1], VIOLET);
                    DrawLine3D(sprite.Bounds[1], sprite.Bounds[2], VIOLET);
                    DrawLine3D(sprite.Bounds[2], sprite.Bounds[3], VIOLET);
                    DrawLine3D(sprite.Bounds[3], sprite.Bounds[0], VIOLET);
                }
            }
        
            // draw 3D models
            {
                auto view = m_Registry.view<TransformComponent, ModelComponent>();
                for (auto entity : view)
                {
                    auto [transform, model] = view.get<TransformComponent, ModelComponent>(entity);
                    
                    transform.PushMatrix();
                    
                        DrawModel(model.ModelData, (Vector3){0.0f, 2.0f, 0.0f}, 1.0f, WHITE); // @TODO: Temp
                    
                    transform.PopMatrix();
                }
            }
        
        
            // draw camera debug
            {
                auto view = m_Registry.view<CameraComponent>();
                for (auto entity : view)
                {
                    Entity entt = {entity, this};
                    auto& cameraComponent = entt.GetComponent<CameraComponent>();
                    
                    if (cameraComponent.Debug) {
                        Renderer::RenderCameraDebugLines(cameraComponent.Camera, VIOLET);
                    }
                }
            }
        
        EndMode3D();
    }
}
