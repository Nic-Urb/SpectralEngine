//
//  Component.cpp
//  SpectralEngine
//
//  Created by Nicolas U on 30.03.24.
//

#include "Component.hpp"

#include "Object.hpp"
#include "Renderer/TextureManager.hpp"

#include "rlgl.h"

namespace Spectral {

// ----- Component -----

    Component::Component(Object& owner) : m_Owner(owner)
    {
    }

    Component::~Component()
    {
    }

// ----- TransformComponent -----

    void TransformComponent::PushMatrix()
    {
        rlPushMatrix();
        rlTranslatef(Transform.Translation.x, Transform.Translation.y, Transform.Translation.z);
        rlRotatef(Transform.Rotation.x, 1, 0, 0);
        rlRotatef(Transform.Rotation.y, 0, 1, 0);
        rlRotatef(Transform.Rotation.z, 0, 0, 1);
        rlScalef(Transform.Scale.x, Transform.Scale.y, Transform.Scale.z);
    }


    void TransformComponent::PopMatrix()
    {
        rlPopMatrix();
    }

// ----- SpriteComponent -----

    void SpriteComponent::OnRender()
    {
        if (!IsTextureReady(SpriteTexture)) {
            return;
        }
        
        // temp
        m_Bounds.x = 0.0f;
        m_Bounds.y = 0.0f;
        m_Bounds.width = SpriteTexture.width;
        m_Bounds.height = SpriteTexture.height;
        
        TransformComponent* transform = m_Owner.GetComponent<TransformComponent>();
        
        if (transform) {
            transform->PushMatrix();
        }
        
        DrawTexturedPlane(SpriteTexture, (Vector3){0.0f, 2.0f, 0.0f}, (Vector2){(float)SpriteTexture.width, (float)SpriteTexture.height}, ConvertToColor());
        
        if (transform) {
            transform->PopMatrix();
        }
    }

    // ImGui color = ::Color / 255.0f  <-> ::Color = ImGui color * 255.0f;
    Color SpriteComponent::ConvertToColor()
    {
        Color temp;
        temp.r = Tint.x * 255.0f;
        temp.g = Tint.y * 255.0f;
        temp.b = Tint.z * 255.0f;
        temp.a = Tint.w * 255.0f;
        
        return temp;
    }

    

}
