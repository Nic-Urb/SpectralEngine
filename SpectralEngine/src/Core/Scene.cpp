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
#include "Physics/PhysicsEngine3D.hpp"

#include "raylib.h"
#include "raymath.h"

#include "box2d/box2d.h"

// Jolt includes
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/MutableCompoundShape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>


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
        m_EntityMap.erase(entity.GetComponent<IDComponent>().ID);
        m_Registry.destroy(entity);
    }

    void Scene::OnRuntimeStart()
    {
        // physics
        {
            // 3D physics
            {
                PhysicsEngine3D::Init();
                
                auto view = m_Registry.view<RigidBody3DComponent>();
                for (auto handle : view)
                {
                    Entity entt {handle, this};
                    
                    auto& rb3d = entt.GetComponent<RigidBody3DComponent>();
                    auto& transform = entt.GetComponent<TransformComponent>();
                    
                    auto& bodyInterface = PhysicsEngine3D::GetPhysicsSystem().GetBodyInterface();
                    
                    JPH::MutableCompoundShapeSettings compoundShapeSettings;
                    
                    // setup shapes
                    
                    if (entt.HasComponent<BoxCollider3DComponent>())
                    {
                        auto& bc3d = entt.GetComponent<BoxCollider3DComponent>();
                        
                        // @NOTE: each physics model needs to be exported in 1,1,1 scale (need to add support for other scale sizes)
                        JPH::BoxShapeSettings boxSettings({1.0f * transform.Scale.x, 1.0f * transform.Scale.y, 1.0f * transform.Scale.z});
                        
                        boxSettings.SetDensity(bc3d.Density);
                        
                        // @TODO: restitution
                        
                        compoundShapeSettings.AddShape({bc3d.Offset.x, bc3d.Offset.y, bc3d.Offset.z}, JPH::Quat::sIdentity(), boxSettings.Create().Get());
                    }
                    
                    // @TODO: setup circle collider
                    
                    Quaternion rotation = QuaternionFromEuler(transform.Rotation.x, transform.Rotation.y, transform.Rotation.z);
                    uint16_t layerIndex = entt.GetComponent<IDComponent>().LayerMask;
                    
                    // setup body
                    JPH::BodyCreationSettings bodySettings(compoundShapeSettings.Create().Get(), {transform.Translation.x, transform.Translation.y, transform.Translation.z}, {rotation.x, rotation.y, rotation.z, rotation.w}, static_cast<JPH::EMotionType>(rb3d.Type), layerIndex);
                    
                    JPH::MassProperties massProperties;
                    massProperties.mMass = rb3d.Mass;
                    bodySettings.mMassPropertiesOverride = massProperties;
                    bodySettings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
                    
                    bodySettings.mAllowSleeping = rb3d.AllowSleep;
                    bodySettings.mLinearDamping = rb3d.LinearDrag;
                    bodySettings.mAngularDamping = rb3d.AngularDrag;
                    bodySettings.mGravityFactor = rb3d.GravityScale;
                    
                    // add body to world
                    JPH::Body* body = bodyInterface.CreateBody(bodySettings);
                    bodyInterface.AddBody(body->GetID(), rb3d.Awake ? JPH::EActivation::Activate : JPH::EActivation::DontActivate);
                    
                    rb3d.RuntimeBody = body;
                }
                
                JPH::PhysicsSystem& physicsSystem = PhysicsEngine3D::GetPhysicsSystem();
                physicsSystem.OptimizeBroadPhase();
            }
            
            // 2D physics
            {
                m_PhysicsWorld = new b2World({0.0f, -10.0f} /*setting up the gravity*/);
                
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
        // 3D physics
        {
            JPH::BodyInterface& bodyInterface = PhysicsEngine3D::GetPhysicsSystem().GetBodyInterface();
            
            auto view = m_Registry.view<RigidBody3DComponent>();
            for (auto handle : view)
            {
                Entity entt = {handle, this};
                
                auto& rb3d = entt.GetComponent<RigidBody3DComponent>();
                
                if (rb3d.RuntimeBody)
                {
                    JPH::Body* body = (JPH::Body*)rb3d.RuntimeBody;
                    bodyInterface.RemoveBody(body->GetID());
                    bodyInterface.DestroyBody(body->GetID());
                    
                    rb3d.RuntimeBody = nullptr;
                }
            }
            
            PhysicsEngine3D::Shutdown();
        }
        
        
        // 2D physics
        {
            delete m_PhysicsWorld;
            m_PhysicsWorld = nullptr;
        }
        
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
        }
        
        // update physics
        {
            // @TODO: Use custom physicsTs for physics 2D and 3D
            
            PhysicsEngine3D::StepPhysics(ts);
            
            // using fewer iterations increases performance but accuracy gonna suffer
            // @NOTE: maybe expose to some physics setting to change the precision
            const int32_t velocityIteration = 6;
            const int32_t positionIteration = 2;
            m_PhysicsWorld->Step(ts, velocityIteration, positionIteration);
            
            
            // update 3D physics
            {
                const auto& bodyInterface = PhysicsEngine3D::GetPhysicsSystem().GetBodyInterface();
                
                auto view = m_Registry.view<RigidBody3DComponent>();
                for (auto handle : view)
                {
                    Entity entt = {handle, this};
                    
                    auto& rb3d = entt.GetComponent<RigidBody3DComponent>();
                    auto& transform = entt.GetComponent<TransformComponent>();
                    
                    JPH::Body* body = (JPH::Body*)rb3d.RuntimeBody;
                    
                    if (!bodyInterface.IsActive(body->GetID())) {
                        continue;
                    }
                        
                    JPH::Vec3 position = body->GetPosition();
                    JPH::Vec3 rotation = body->GetRotation().GetEulerAngles();
                    
                    transform.Translation = {position.GetX(), position.GetY(), position.GetZ()};
                    transform.Rotation = {rotation.GetX(), rotation.GetY(), rotation.GetZ()};
                }

            }
            
            // update 2D physics
            {
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
                        
                        // @TODO: temp solution
                        Color color;
                        color.r = model.Tint.x * 255.0f;
                        color.g = model.Tint.y * 255.0f;
                        color.b = model.Tint.z * 255.0f;
                        color.a = model.Tint.w * 255.0f;
                        DrawModel(model.ModelData, (Vector3){0.0f, 0.0f, 0.0f}, 1.0f, color);
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
                    
                    // @TODO: temp solution
                    Color color;
                    color.r = model.Tint.x * 255.0f;
                    color.g = model.Tint.y * 255.0f;
                    color.b = model.Tint.z * 255.0f;
                    color.a = model.Tint.w * 255.0f;
                    DrawModel(model.ModelData, (Vector3){0.0f, 0.0f, 0.0f}, 1.0f, color);
                    
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
