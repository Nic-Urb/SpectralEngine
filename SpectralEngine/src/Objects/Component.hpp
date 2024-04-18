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
        Object& GetOwner() const { return m_Owner; }
        
    private:
        Object& m_Owner;
        
    };


    class TransformComponent : public Component
    {
    public:
        DECLARE_COMPONENT(TransformComponent)
        
        struct Transform2D {
            Vector2 Translation = {0.0f, 0.0f};
            float Rotation = 0.0f;
            Vector2 Scale = {1.0f, 1.0f};
        };
        
        // -- set relative to current transform 0 , 0 , 0 means nothing changed
        void SetTransform(const Vector2& translate, float rotate, const Vector2& scale);
        
        void PushMatrix();
        void PopMatrix();
        
        Vector2& GetTranslation() { return m_Transform.Translation; }
        float& GetRotation() { return m_Transform.Rotation; }
        Vector2& GetScale() { return m_Transform.Scale; }
        
    private:
        Transform2D m_Transform;
        
    };


    class SpriteComponent : public Component
    {
    public:
        DECLARE_COMPONENT(SpriteComponent)
        
        void OnRender() override;
        
        void SetSprite(const Texture2D& texture) {
            m_Texture = texture;
            // temp
            m_Bounds.x = 0.0f;
            m_Bounds.y = 0.0f;
            m_Bounds.width = m_Texture.width;
            m_Bounds.height = m_Texture.height;
        }
        void SetBounds(const Rectangle& bounds) { m_Bounds = bounds; }
        
        float* GetTint() { return m_Tint; }
        
    private:
        Texture2D m_Texture;
        Rectangle m_Bounds;
        float     m_Tint[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // RAYLIB::WHITE
        Vector2 m_Postion = {10.0f,10.0f}; // temp
        
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
    };

}
