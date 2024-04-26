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

    void TransformComponent::SetTransform(const Vector2& translate, float rotate, const Vector2& scale)
    {
        m_Transform.Translation.x += translate.x;
        m_Transform.Translation.y += translate.y;
        m_Transform.Rotation += rotate;
        m_Transform.Scale.x += scale.x;
        m_Transform.Scale.y += scale.y;
    }

    void TransformComponent::PushMatrix()
    {
        rlPushMatrix();
        rlTranslatef(m_Transform.Translation.x, m_Transform.Translation.y, 0);
        rlRotatef(m_Transform.Rotation, 0, 0, 1);
        rlScalef(m_Transform.Scale.x, m_Transform.Scale.y, 0);
    }


    void TransformComponent::PopMatrix()
    {
        rlPopMatrix();
    }

// ----- SpriteComponent -----

    void SpriteComponent::OnRender(const Camera3D& camera)
    {
        if (!IsTextureReady(m_Texture)) {
            return;
        }
        
        TransformComponent* transform = GetOwner().GetComponent<TransformComponent>();
        
        if (transform) {
            transform->PushMatrix();
        }
        
        //const Rectangle dest = { 0, 0, fabsf(m_Bounds.width * 0.1f/* Scale */), fabsf(m_Bounds.height * 0.1f/* Scale */) };
        
        const Rectangle dest = { m_Bounds.x, m_Bounds.y, fabsf(m_Bounds.width * 0.1f/* Scale */), fabsf(m_Bounds.height * 0.1f/* Scale */) };
        
        //DrawTexturePro(m_Texture, m_Bounds, dest, Vector2{ dest.width * 0.5f, dest.height* 0.5f}, 0, ConvertToColor());
        DrawBillboard(camera, m_Texture, (Vector3){0.0f, 2.0f, 0.0f}, 150.0f, ConvertToColor()); // @TODO: Do this properly
    
        if (transform) {
            transform->PopMatrix();
        }
    }

    // ImGui color = ::Color / 255.0f  <-> ::Color = ImGui color * 255.0f;
    Color SpriteComponent::ConvertToColor()
    {
        Color temp;
        temp.r = m_Tint[0] * 255.0f;
        temp.g = m_Tint[1] * 255.0f;
        temp.b = m_Tint[2] * 255.0f;
        temp.a = m_Tint[3] * 255.0f;
        
        return temp;
    }

    

}
