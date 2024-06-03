//
//  Renderer.cpp
//  SpectralEngine
//
//  Created by Nicolas U on 02.06.24.
//

#include "Renderer.hpp"

#include "rlgl.h"
#include "raymath.h"

#include "Math/Math.hpp"


namespace Spectral {
    
    void Renderer::RenderTexturedPlane(const Texture &texture, const Vector4 &tint)
    {
        Color color;
        color.r = tint.x * 255.0f;
        color.g = tint.y * 255.0f;
        color.b = tint.z * 255.0f;
        color.a = tint.w * 255.0f;
        
        DrawTexturedPlane(texture, (Vector3){0.0f, 2.0f, 0.0f}, (Vector2){(float)texture.width, (float)texture.height}, color);
    }

    void Renderer::RenderCameraDebugLines(std::shared_ptr<RuntimeCamera> camera, Color color)
    {
        Math::Frustum frustum;
        Math::ExtractFrustrum(camera->GetCameraProjectionMatrix(GetRenderWidth()/GetRenderHeight()), camera->GetCameraViewMatrix(), &frustum);
                
        DrawSphere(camera->GetCamera3D().position, 0.2f, GREEN);
        
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
}
