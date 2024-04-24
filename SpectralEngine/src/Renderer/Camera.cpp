//
//  Camera.cpp
//  SpectralEngine
//
//  Created by Nicolas U on 20.04.24.
//

#include "Camera.hpp"
#include "raymath.h"

namespace Spectral {
    Camera::Camera(Camera2D& camera) : m_Camera2D(camera)
    {}

    void Camera::BlendCameras(Camera2D& otherCamera, float delta) // @TODO: Test this function inside a Sandbox app !!
    {
        // using linear interpolation between 2 vectors
        m_Camera2D.target = Vector2Lerp(m_Camera2D.target, otherCamera.target, delta);
    }

    void Camera::FocusCamera(const Vector2& position, float width, float height)
    {
        m_Camera2D.offset = (Vector2){width/2.0f, height/2.0f};
        m_Camera2D.target = position;
    }
}



