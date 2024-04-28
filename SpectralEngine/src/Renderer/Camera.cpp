//
//  Camera.cpp
//  SpectralEngine
//
//  Created by Nicolas U on 20.04.24.
//

#include "Camera.hpp"

#include "raymath.h"
#include "rlgl.h"

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

    Matrix Camera::GetCameraViewMatrix()
    {
        return MatrixLookAt(m_Camera3D.position, m_Camera3D.target, m_Camera3D.up);
    }

    Matrix Camera::GetCameraProjectionMatrix(float aspect)
    {
        if (m_Camera3D.projection == CAMERA_PERSPECTIVE)
        {
            return MatrixPerspective(m_Camera3D.fovy*DEG2RAD, aspect, RL_CULL_DISTANCE_NEAR, RL_CULL_DISTANCE_FAR);
        }
        else if (m_Camera3D.projection == CAMERA_ORTHOGRAPHIC)
        {
            double top = m_Camera3D.fovy/2.0;
            double right = top*aspect;

            return MatrixOrtho(-right, right, -top, top, RL_CULL_DISTANCE_NEAR, RL_CULL_DISTANCE_FAR);
        }

        return MatrixIdentity();
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



