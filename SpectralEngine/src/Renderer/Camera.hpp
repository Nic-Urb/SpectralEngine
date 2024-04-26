//
//  Camera.hp
//  SpectralEngine
//
//  Created by Nicolas U on 04.04.24.
//
#pragma once

#include "pch.h"
#include "Core/Timestep.h"

#include "raylib.h"

namespace Spectral {

    class Camera
    {
    public:
        Camera() = default;
        Camera(const Camera3D& camera);
        virtual ~Camera() = default;
        
        virtual void OnUpdate(Timestep ts) = 0;
        
        // shared functions
        void ChangeProjection(int projection); // Projection : CAMERA_PERSPECTIVE, CAMERA_ORTHOGRAPHIC
        
        void BlendCameras(Camera3D& otherCamera, float delta);
        const Camera3D& GetCamera3D() { return m_Camera3D; }
        
    protected:
        Camera3D m_Camera3D = {(Vector3){0.0f, 20.0f, 20.0f}, (Vector3){0.0f, 10.0f, 0.0f}, (Vector3){0.0f, 1.0f, 0.0f}, 45.0f, CAMERA_PERSPECTIVE};
        
    protected:
        // focus camera on some position
        virtual void FocusCamera(const Vector3& position, float width, float height); // @TODO: Define this function

    };
}
