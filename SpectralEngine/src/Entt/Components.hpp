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
        Vector3   Bounds[4]; // @TODO: we actually don't need this anymore, after fixing transform component
    };

    struct ModelComponent
    {
        Model     ModelData;
        Texture2D Materials[4];
        Vector4   Tint;
        bool      Transparency = false;
    };

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

    struct HDRIComponent
    {
    };

    struct LightComponent
    {
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
