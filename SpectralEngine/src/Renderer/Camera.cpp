//
//  Camera.cpp
//  SpectralEngine
//
//  Created by Nicolas U on 20.04.24.
//

#include "Camera.hpp"
#include "raymath.h"

namespace Spectral {
    Camera::Camera(const Camera3D& camera) : m_Camera3D(camera)
    {}

    void Camera::ChangeProjection(int projection)
    {
        if (projection == CAMERA_PERSPECTIVE || projection == CAMERA_ORTHOGRAPHIC) 
        {
            m_Camera3D.projection = projection;
        }
    }

    void Camera::BlendCameras(Camera3D& otherCamera, float delta) // @TODO: Test this function inside a Sandbox app !!
    {
        // using linear interpolation between two 3d vectors
        m_Camera3D.target = Vector3Lerp(m_Camera3D.target, otherCamera.target, delta);
    }

    void Camera::FocusCamera(const Vector3& position, float width, float height) // @TODO: 3D : define this functions
    {
       // m_Camera2D.offset = (Vector2){width/2.0f, height/2.0f};
       // m_Camera2D.target = position;
    }
}



