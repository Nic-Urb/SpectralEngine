//
//  Components.hpp
//  SpectralEngine
//
//  Created by Nicolas U on 01.06.24.
//
#pragma once

#include "pch.h"

#include "raylib.h"
#include "Core/UUID.hpp"
#include "Renderer/RuntimeCamera.hpp"

namespace Spectral {

    struct IDComponent
    {
        UUID ID;
        std::string Name;
        
        IDComponent(UUID uuid, const std::string& name);
    };
    
    struct TransformComponent
    {
        Vector3 Translation = {0.0f, 0.0f, 0.0f};
        Vector3 Rotation    = {0.0f, 0.0f, 0.0f};
        Vector3 Scale       = {1.0f, 1.0f, 1.0f};
        
        void PushMatrix();
        void PopMatrix();
    };

    struct SpriteComponent
    {
        Texture2D SpriteTexture;
        Vector4   Tint;
        Vector3   Bounds[4];
    };

    struct ModelComponent
    {
        Model     ModelData;
        Texture2D Materials[4];
        Vector4   Tint;
        bool      Transparency = false;
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
        
        CameraComponent();
    };

    struct AnimationComponent
    {};

    struct NativeScriptComponent
    {};
}
