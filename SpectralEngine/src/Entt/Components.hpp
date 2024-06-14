//
//  Components.hpp
//  SpectralEngine
//
//  Created by Nicolas U on 01.06.24.
//
#pragma once

#include "pch.h"

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "sol.hpp"

#include "Core/UUID.hpp"
#include "Renderer/RuntimeCamera.hpp"


namespace Spectral {

    struct IDComponent
    {
        UUID ID;
        std::string Name;
        uint16_t LayerMask = 1; // used for physics 3d collision
        
        IDComponent(UUID uuid, const std::string& name)
                : ID(uuid), Name(name) {}
    };
    
    struct TransformComponent
    {
        Vector3 Translation = {0.0f, 0.0f, 0.0f};
        Vector3 Rotation    = {0.0f, 0.0f, 0.0f};
        Vector3 Scale       = {1.0f, 1.0f, 1.0f};
        
        
        Matrix GetTransform() const
        {
            // calculate transformation matrix (scale * rotate * translate)
            Matrix rotation = MatrixRotateZYX(Rotation);

            return MatrixMultiply(MatrixMultiply(MatrixScale(Scale.x, Scale.y, Scale.z), rotation)
                                  , MatrixTranslate(Translation.x, Translation.y, Translation.z));
        }
    };

    struct SpriteComponent
    {
        Texture2D SpriteTexture;
        Vector4   Tint = {1.0f, 1.0f, 1.0f, 1.0f};
    };

    struct ModelComponent
    {
        // @TODO: add different culling modes
        Model     ModelData;
        Vector4   Tint = {1.0, 1.0f, 1.0f, 1.0f};
        bool      Transparency = false;
    };

    // 2D Physics

    struct RigidBody2DComponent
    {
        enum class BodyType { Static = 0, Kinematic, Dynamic };
        BodyType Type = BodyType::Static;
        bool FixedRotation = false;
        bool AllowSleep = true;
        bool Awake = true;
        float GravityScale = 1.0f;
        
        void* RuntimeBody = nullptr;
    };

    struct BoxCollider2DComponent
    {
        Vector2 Offset = {0.0f, 0.0f};
        Vector2 Size = {50.0f, 50.0f};
        
        float Density = 1.0f;
        float Friction = 0.5f;
        float Restitution = 0.0f;
    };

    // 3D Physics

    struct RigidBody3DComponent
    {
        enum class BodyType { Static = 0, Kinematic, Dynamic };
        BodyType Type = BodyType::Static;
        
        bool AllowSleep = true;
        bool Awake = true;
        float Mass = 1.0f;
        float LinearDrag = 0.0f;
        float AngularDrag = 0.0f;
        float GravityScale = 1.0f;
        
        void* RuntimeBody = nullptr;
        
        // @TODO: add more options (https://jrouwe.github.io/JoltPhysics/index.html#bodies)
    };

    struct BoxCollider3DComponent
    {
        Vector3 Offset = {0.0f, 0.0f, 0.0f};
        
        float Density = 1.0f;
        float Friction = 0.5f;
        float Restitution = 0.0f;
    };

    struct SphereCollider3DComponent
    {
        Vector3 Offset = {0.0f, 0.0f, 0.0f};
        float Radius = 50.0f;
                
        float Density = 1.0f;
        float Friction = 0.5f;
        float Restitution = 0.0f;
    };

    struct WaterBuoyancy3DComponent // @SEE: https://github.com/jrouwe/JoltPhysics/tree/master/Samples/Tests/Water
    {
        float Density = 1.0f;
    };

    struct LightComponent // @TODO: Use with some light manager
    {
        enum class LightType { Directional = 0, Point, Spot };
        LightType Type;
        
        bool Enabled = true;
        bool AllowTemperature = false;
        
        float Attenuation = 1.0f;
        float Range = 1.0f;
        float Temperature = 500.0f; // in kelvins
        
        Vector4 Color = {1.0f, 1.0f, 1.0f, 1.0f};
    };

    struct CameraComponent
    {
        std::shared_ptr<RuntimeCamera> Camera;
        bool Active = false;
        bool Debug =  false;
        
        CameraComponent() {  Camera = std::make_shared<RuntimeCamera>(); }
    };

    struct LuaScriptComponent
    {
        sol::table self;
        std::string ScriptPath;
    };
    
    struct AnimationComponent
    {};
}
