//
//  Scene.cpp
//  SpectralEngine
//
//  Created by Nicolas U on 05.04.24.
//

#include "Scene.hpp"

#include "Entt/Entity.hpp"

#include "Renderer/Renderer.hpp"

#include "Scripting/ScriptingEngine.hpp"

#include "raylib.h"
#include "raymath.h"

#include "box2d/box2d.h"

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
    {
        m_PhysicsWorld = new b2World({0.0f, -15.0f} /*setting up the gravity*/);
        
        // physics
        {
            auto view = m_Registry.view<RigidBody2DComponent>();
            for (auto handle : view)
            {
                Entity entt = {handle, this};
                
                auto& rb2d = entt.GetComponent<RigidBody2DComponent>();
                auto& transform = entt.GetComponent<TransformComponent>();
                
                b2BodyDef bodyDef;
                bodyDef.type = (b2BodyType)rb2d.Type;
                bodyDef.position.Set(transform.Translation.x, transform.Translation.y);
                bodyDef.angle = transform.Rotation.z;
                
                bodyDef.fixedRotation = rb2d.FixedRotation;
                bodyDef.allowSleep = rb2d.AllowSleep;
                bodyDef.awake = rb2d.Awake;
                bodyDef.gravityScale = rb2d.GravityScale;
                
                b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
                rb2d.RuntimeBody = body;
                
                if (entt.HasComponent<BoxCollider2DComponent>())
                {
                    auto& bc2d = entt.GetComponent<BoxCollider2DComponent>();
                    
                    b2PolygonShape boxShape;
                    boxShape.SetAsBox((bc2d.Size.x / 2) * transform.Scale.x, (bc2d.Size.y / 2) * transform.Scale.y, b2Vec2(bc2d.Offset.x, bc2d.Offset.y), 0.0f);
                    
                    b2FixtureDef fixtureDef;
                    fixtureDef.shape = &boxShape;
                    fixtureDef.density = bc2d.Density;
                    fixtureDef.friction = bc2d.Friction;
                    fixtureDef.restitution = bc2d.Restitution;
                    
                    body->CreateFixture(&fixtureDef);
                }
                
                // @TODO: Circle collider
            }
        }
        
        // lua scripts
        {
            auto view = m_Registry.view<LuaScriptComponent>();
            for (auto handle : view)
            {
                Entity entt = {handle, this};
                
                ScriptingEngine::OnCreate(entt);
            }
        }
    }

    void Scene::OnRuntimeEnd()
    {
        delete m_PhysicsWorld;
        m_PhysicsWorld = nullptr;
        // @TODO: script on destroy
        /*m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc) {
         //ScriptingEngine::OnDestroy(entity);
        });
        */
    }

    void Scene::OnUpdateRuntime(Timestep ts)
    {
        // NOTE: Order is important script -> physics
        
        // update scripts
        {
            // lua scripts
            auto view = m_Registry.view<LuaScriptComponent>();
            for (auto handle : view)
            {
                Entity entt = {handle, this};
                ScriptingEngine::OnUpdate(entt, ts);
            }
            
            // @TODO: native scripts
        }
        
        // update physics
        {
            // using fewer iterations increases performance but accuracy gonna suffer
            // @Note: maybe expose to some physics setting to change the precision
            const int32_t velocityIteration = 6;
            const int32_t positionIteration = 2;
            
            m_PhysicsWorld->Step(ts, velocityIteration, positionIteration);
            
            auto view = m_Registry.view<RigidBody2DComponent>();
            for (auto handle : view)
            {
                Entity entt = {handle, this};
                
                auto& rb2d = entt.GetComponent<RigidBody2DComponent>();
                auto& transform = entt.GetComponent<TransformComponent>();
                
                b2Body* body = (b2Body*)rb2d.RuntimeBody;
                
                const auto& position = body->GetPosition();
                transform.Translation.x = position.x;
                transform.Translation.y = position.y;
                transform.Rotation.z = body->GetAngle();
            }
        }
        
        // update camera
        {
            auto view = m_Registry.view<TransformComponent, CameraComponent>();
            for (auto handle : view)
            {
                auto [transform, camera] = view.get<TransformComponent, CameraComponent>(handle);
                
                if (camera.Active) {
                    m_RuntimeCamera = camera.Camera;
                    m_RuntimeCamera->SetTransform(transform.GetTransform());
                } else {
                    m_RuntimeCamera = nullptr;
                }
            }
        }
    }

    void Scene::OnRenderRuntime()
    {
        if (m_RuntimeCamera)
        {
            BeginMode3DM(m_RuntimeCamera->GetCamera3D(), m_RuntimeCamera->GetTransform());
            
                // draw sprites
                {
                    auto view = m_Registry.view<TransformComponent, SpriteComponent>();
                    for (auto handle : view)
                    {
                        auto [transform, sprite] = view.get<TransformComponent, SpriteComponent>(handle);
                    
                        Renderer::RenderTexturedPlane(sprite.SpriteTexture, transform.GetTransform(), sprite.Tint);
                    }
                }
            
                // draw 3D models
                {
                    auto view = m_Registry.view<TransformComponent, ModelComponent>();
                    for (auto handle : view)
                    {
                        auto [transform, model] = view.get<TransformComponent, ModelComponent>(handle);

                        model.ModelData.transform = transform.GetTransform();
                        
                        // @Note: do not modify position and scale values here, the transform matrix is paased from a model
                        DrawModel(model.ModelData, (Vector3){0.0f, 0.0f, 0.0f}, 1.0f, WHITE);
                    }
                }
            
            EndMode3D();
        }
    }

    void Scene::OnUpdateEditor(Timestep ts)
    {
    }

    void Scene::OnRenderEditor(EditorCamera& camera)
    {
        BeginMode3D(camera.GetCamera3D());
            
            // draw grid
            DrawGrid(20, 10.0f);
        
            // draw sprites
            {
                auto view = m_Registry.view<TransformComponent, SpriteComponent>();
                for (auto handle : view)
                {
                    auto [transform, sprite] = view.get<TransformComponent, SpriteComponent>(handle);
                    
                    Renderer::RenderTexturedPlane(sprite.SpriteTexture, transform.GetTransform(), sprite.Tint);
                    
                    // @DEBUG
                    DrawCubeWiresM(transform.GetTransform(), (Vector3){50.0f, 50.0f ,1.0f}, VIOLET);
                }
            }
        
            // draw 3D models
            {
                auto view = m_Registry.view<TransformComponent, ModelComponent>();
                for (auto handle : view)
                {
                    auto [transform, model] = view.get<TransformComponent, ModelComponent>(handle);
                    
                    model.ModelData.transform = transform.GetTransform();
                    
                    // @Note: do not modify position and scale values here, the transform matrix is paased from a model
                    DrawModel(model.ModelData, (Vector3){0.0f, 0.0f, 0.0f}, 1.0f, WHITE);
                    
                    // @DEBUG
                    ///DrawBoundingBox(GetMeshBoundingBox(model.ModelData.meshes[0]), VIOLET);
                }
            }
        
        
            // draw camera debug
            {
                auto view = m_Registry.view<CameraComponent>();
                for (auto handle : view)
                {
                    Entity entt = {handle, this};
                    auto& cameraComponent = entt.GetComponent<CameraComponent>();
                    
                    if (cameraComponent.Debug) {
                        Renderer::RenderCameraDebugLines(cameraComponent.Camera, VIOLET);
                    }
                }
            }
        
        EndMode3D();
    }
}
