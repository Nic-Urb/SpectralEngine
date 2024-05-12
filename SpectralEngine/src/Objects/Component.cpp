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
        rlTranslatef(m_Transform.Translation.x, m_Transform.Translation.y, m_Transform.Translation.z);
        rlRotatef(m_Transform.Rotation.x, 1, 0, 0);
        rlRotatef(m_Transform.Rotation.y, 0, 1, 0);
        rlRotatef(m_Transform.Rotation.z, 0, 0, 1);
        rlScalef(m_Transform.Scale.x, m_Transform.Scale.y, m_Transform.Scale.z);
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
        
        TransformComponent* transform = m_Owner.GetComponent<TransformComponent>();
        
        if (transform) {
            transform->PushMatrix();
        }
        
        //const Rectangle dest = { 0, 0, fabsf(m_Bounds.width * 0.1f/* Scale */), fabsf(m_Bounds.height * 0.1f/* Scale */) };
        
        const Rectangle dest = { m_Bounds.x, m_Bounds.y, fabsf(m_Bounds.width * 0.1f/* Scale */), fabsf(m_Bounds.height * 0.1f/* Scale */) };
        
        //DrawTexturePro(m_Texture, m_Bounds, dest, Vector2{ dest.width * 0.5f, dest.height* 0.5f}, 0, ConvertToColor());
        DrawBillboard(camera, m_Texture, (Vector3){0.0f, 2.0f, 0.0f}, 150.0f, ConvertToColor()); // @TODO: Do this properly
        // @TODO: Switch to plane !
        
        
        if (transform) {
            transform->PopMatrix();
        }
    }

    // ImGui color = ::Color / 255.0f  <-> ::Color = ImGui color * 255.0f;
    Color SpriteComponent::ConvertToColor()
    {
        Color temp;
        temp.r = m_Tint.x * 255.0f;
        temp.g = m_Tint.y * 255.0f;
        temp.b = m_Tint.z * 255.0f;
        temp.a = m_Tint.w * 255.0f;
        
        return temp;
    }

    

}
