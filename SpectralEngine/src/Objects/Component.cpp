//
//  Component.cpp
//  SpectralEngine
//
//  Created by Nicolas U on 30.03.24.
//

#include "Component.hpp"

#include "Object.hpp"
#include "Renderer/TextureManager.hpp"
#include "Math/Math.hpp"

#include "rlgl.h"
#include "raymath.h"

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

    void SpriteComponent::OnUpdate(Timestep ts)
    {
        TransformComponent* transform = m_Owner.GetComponent<TransformComponent>();
        
        Vector3 position;
        Vector2 size;
        Vector3 rotation;
        
        if (transform)
        {
            position = transform->Transform.Translation;
            size = (Vector2){(float)SpriteTexture.width * transform->Transform.Scale.x, (float)SpriteTexture.height * transform->Transform.Scale.z};
            rotation = transform->Transform.Rotation;
        } else {
            position = m_OriginPosition;
            size = (Vector2){(float)SpriteTexture.width, (float)SpriteTexture.height};
            rotation = (Vector3){0.0f, 0.0f, 0.0f};
        }
        
        Bounds[0] = (Vector3){position.x - size.x / 2, position.y, position.z - size.y / 2};        // Top left corner
        Bounds[1] = (Vector3){position.x + size.x / 2, position.y, position.z - size.y / 2};        // Top right corner
        Bounds[2] = (Vector3){position.x + size.x / 2, position.y, position.z + size.y / 2};        // Bottom right corner
        Bounds[3] = (Vector3){position.x - size.x / 2, position.y, position.z + size.y / 2};        // Bottom left corner
        
        // Only calculate if we have some rotation values
        if (rotation.x != 0.0f || rotation.y != 0.0f || rotation.z != 0.0f)
        {
            Matrix rotX = MatrixRotateX(rotation.x * DEG2RAD);
            Matrix rotY = MatrixRotateY(rotation.y * DEG2RAD);
            Matrix rotZ = MatrixRotateZ(rotation.z * DEG2RAD);
            
            // Combine the rotation matrices (rotZ * (rotY * rotX))
            Matrix rotationMatrix = MatrixMultiply(rotZ, MatrixMultiply(rotY, rotX));
            
            // Apply the combined rotation matrix to each corner
            for (int i = 0; i < 4; ++i)
            {
                Vector3 corner = (Vector3){Bounds[i].x - position.x, Bounds[i].y - position.y, Bounds[i].z - position.z};
                Vector3 transformed = Vector3Transform(corner, rotationMatrix);
                Bounds[i] = (Vector3){transformed.x + position.x, transformed.y + position.y, transformed.z + position.z};
            }
        }
    }

    void SpriteComponent::OnRender()
    {
        if (!IsTextureReady(SpriteTexture)) {
            return;
        }
        
        TransformComponent* transform = m_Owner.GetComponent<TransformComponent>();
        
        if (transform) {
            transform->PushMatrix();
        }
        
        DrawTexturedPlane(SpriteTexture, m_OriginPosition, (Vector2){(float)SpriteTexture.width, (float)SpriteTexture.height}, ConvertToColor());
        
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

// ----- CameraComponent -----

    static void DrawCameraDebug(Math::Frustum frustum, Color color)
    {
        const Vector4 rightPlane = frustum.Planes[4];
        const Vector4 leftPlane = frustum.Planes[5];
        const Vector4 topPlane = frustum.Planes[3];
        const Vector4 bottomPlane = frustum.Planes[2];
        const Vector4 nearPlane = frustum.Planes[0];
        const Vector4 farPlane = frustum.Planes[1];
        
        // Intersect the planes to find the corners
        const Vector3 nearTopLeft = Math::Intersection(nearPlane, topPlane, leftPlane);
        const Vector3 nearTopRight = Math::Intersection(nearPlane, topPlane, rightPlane);
        const Vector3 nearBottomLeft = Math::Intersection(nearPlane, bottomPlane, leftPlane);
        const Vector3 nearBottomRight = Math::Intersection(nearPlane, bottomPlane, rightPlane);

        const Vector3 farTopLeft = Math::Intersection(farPlane, topPlane, leftPlane);
        const Vector3 farTopRight = Math::Intersection(farPlane, topPlane, rightPlane);
        const Vector3 farBottomLeft = Math::Intersection(farPlane, bottomPlane, leftPlane);
        const Vector3 farBottomRight = Math::Intersection(farPlane, bottomPlane, rightPlane);
        
        // Draw camera frustum
        rlBegin(RL_LINES);
            rlColor4ub(color.r, color.g, color.b, color.a);
        
            // Near plane
            rlVertex3f(nearTopLeft.x, nearTopLeft.y, nearTopLeft.z);
            rlVertex3f(nearTopRight.x, nearTopRight.y, nearTopRight.z);

            rlVertex3f(nearTopRight.x, nearTopRight.y, nearTopRight.z);
            rlVertex3f(nearBottomRight.x, nearBottomRight.y, nearBottomRight.z);

            rlVertex3f(nearBottomRight.x, nearBottomRight.y, nearBottomRight.z);
            rlVertex3f(nearBottomLeft.x, nearBottomLeft.y, nearBottomLeft.z);

            rlVertex3f(nearBottomLeft.x, nearBottomLeft.y, nearBottomLeft.z);
            rlVertex3f(nearTopLeft.x, nearTopLeft.y, nearTopLeft.z);

            // Far plane
            rlVertex3f(farTopLeft.x, farTopLeft.y, farTopLeft.z);
            rlVertex3f(farTopRight.x, farTopRight.y, farTopRight.z);

            rlVertex3f(farTopRight.x, farTopRight.y, farTopRight.z);
            rlVertex3f(farBottomRight.x, farBottomRight.y, farBottomRight.z);

            rlVertex3f(farBottomRight.x, farBottomRight.y, farBottomRight.z);
            rlVertex3f(farBottomLeft.x, farBottomLeft.y, farBottomLeft.z);
        
            rlVertex3f(farBottomLeft.x, farBottomLeft.y, farBottomLeft.z);
            rlVertex3f(farTopLeft.x, farTopLeft.y, farTopLeft.z);

            // Connecting near and far planes
            rlVertex3f(nearTopLeft.x, nearTopLeft.y, nearTopLeft.z);
            rlVertex3f(farTopLeft.x, farTopLeft.y, farTopLeft.z);

            rlVertex3f(nearTopRight.x, nearTopRight.y, nearTopRight.z);
            rlVertex3f(farTopRight.x, farTopRight.y, farTopRight.z);

            rlVertex3f(nearBottomRight.x, nearBottomRight.y, nearBottomRight.z);
            rlVertex3f(farBottomRight.x, farBottomRight.y, farBottomRight.z);

            rlVertex3f(nearBottomLeft.x, nearBottomLeft.y, nearBottomLeft.z);
            rlVertex3f(farBottomLeft.x, farBottomLeft.y, farBottomLeft.z);

        rlEnd();
    }

    void CameraComponent::OnConstruct()
    {
        Camera = std::make_shared<RuntimeCamera>();
    }

    void CameraComponent::OnRender()
    {
        if (!Camera) {
            return;
        }
        // @TODO: Debug camera is not being displayed in correct size (compare matrices)
        Math::Frustum frustum;
        Math::ExtractFrustrum(Camera->GetCameraProjectionMatrix(GetRenderWidth()/GetRenderHeight()), Camera->GetCameraViewMatrix(), &frustum);
        
        DrawCameraDebug(frustum, VIOLET);
        DrawSphere(Camera->GetCamera3D().position, 0.2f, GREEN);
    }

}
