//
//  Component.hpp
//  SpectralEngine
//
//  Created by Nicolas U on 30.03.24.
//
#pragma once

#include "pch.h"

#include "raylib.h"
#include "HashString.cpp"
#include "Core/Timestep.h"
#include "Renderer/RuntimeCamera.hpp"

// Using macro at compile time to declare component properties.
// std::string_view instead of string, to avoid errors in older C++, in C++20 it wouldn't be necessary
// https://stackoverflow.com/questions/27123306/is-it-possible-to-use-stdstring-in-a-constant-expression
#define DECLARE_COMPONENT(T) \
    static constexpr std::string_view GetComponentName() { return #T;} \
    static constexpr uint32_t GetComponentId() { \
        return Hash::HashString(#T); } \
    T(Object& owner) : Component(owner) {}


namespace Spectral {

    class Object; // fwd declaration

    class Component
    {
    public:
        Component(Object& owner);
        ~Component();
        
        virtual void OnConstruct() {}
        virtual void OnDestroy() {}
        virtual void OnRender() {}
        virtual void OnUpdate(Timestep ts) {}
        
    protected:
        Object& m_Owner;
        
    };


    class TransformComponent : public Component
    {
    public:
        DECLARE_COMPONENT(TransformComponent)
        
        struct Transform2D {
            Vector3 Translation = {0.0f, 0.0f, 0.0f};
            Vector3 Rotation = {0.0f, 0.0f, 0.0f};
            Vector3 Scale = {1.0f, 1.0f, 1.0f};
        };
        
        Transform2D Transform;
        
        void PushMatrix();
        void PopMatrix();
        
    };


    class SpriteComponent : public Component
    {
    public:
        DECLARE_COMPONENT(SpriteComponent)
        
        Texture2D SpriteTexture;
        Vector4   Tint = (Vector4){1.0f, 1.0f, 1.0f, 1.0f}; // RAYLIB::WHITE
        
        void OnRender() override;

    private:
        Rectangle m_Bounds;
        
    private:
        Color ConvertToColor();
        
    };

    class AnimationComponent : public Component
    {
    public:
        DECLARE_COMPONENT(AnimationComponent)
    };

    class CameraComponent : public Component
    {
    public:
        DECLARE_COMPONENT(CameraComponent)
        
        std::shared_ptr<RuntimeCamera> Camera;
        bool Active = false;
        
        void OnConstruct() override { Camera = std::make_shared<RuntimeCamera>(); }

    };

}
