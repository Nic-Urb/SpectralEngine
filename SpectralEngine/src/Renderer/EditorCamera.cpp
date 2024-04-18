//
//  EditorCamera.cpp
//  SpectralEngine
//
//  Created by Nicolas U on 04.04.24.
//

#include "EditorCamera.hpp"

#include "rlgl.h"
#include "raymath.h"
//#include "rcamera.h" @TODO: .

namespace Spectral {

    EditorCamera::EditorCamera()
    {
        m_Camera.zoom = 1.0f;
    }

    void EditorCamera::CameraMove(const Vector2& delta)
    {
        if (IsKeyDown(KEY_W)) {
            m_Camera.target.y -= m_MoveSpeed;
        } else if (IsKeyDown(KEY_A)) {
            m_Camera.target.x -= m_MoveSpeed;
        } else if (IsKeyDown(KEY_S)) {
            m_Camera.target.y += m_MoveSpeed;
        } else if (IsKeyDown(KEY_D)) {
            m_Camera.target.x += m_MoveSpeed;
        } else {
            Vector2 deltaScale = Vector2Scale(delta, -1.0f/m_Camera.zoom); // @TODO: Try without scaling to camera zoom value !
            m_Camera.target = Vector2Add(m_Camera.target, deltaScale);
        }
    }

    void EditorCamera::CameraZoom(const Vector2& delta, float wheel)
    {
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), m_Camera);
                
        m_Camera.offset = GetMousePosition();
        // Set the target to match, so that the camera maps the world space point
        // under the cursor to the screen space point under the cursor at any zoom
        m_Camera.target = mouseWorldPos;

        m_Camera.zoom += (wheel*m_ZoomSpeed);
        if (m_Camera.zoom < m_ZoomSpeed) { m_Camera.zoom = m_ZoomSpeed; }
    }
    
    void EditorCamera::OnUpdate(Timestep ts)
    {
        const Vector2 delta = GetMouseDelta();
        const float mouseWheel = GetMouseWheelMove();
        
        if (mouseWheel != 0) {
            CameraZoom(delta, mouseWheel);
        }
        
        if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) { // support right mouse hold and wasd keyboard to move
            CameraMove(delta);
        }
        
        // Reset camera zoom, position and rotation
        if (IsKeyDown(KEY_R)) {
            m_Camera.zoom = 1.0f;
            m_Camera.rotation = 0.0f;
            // @TODO: reset to origin position which is 0,0
        }
        
    }

}
