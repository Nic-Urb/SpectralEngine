//
//  EditorCamera.cpp
//  SpectralEngine
//
//  Created by Nicolas U on 04.04.24.
//

#include "EditorCamera.hpp"

#include "rlgl.h"
#include "raymath.h"

namespace Spectral {

    EditorCamera::EditorCamera()
    {
        m_Camera2D.zoom = 1.0f;
    }

    void EditorCamera::MoveCamera(const Vector2& delta)
    {
        if (IsKeyDown(KEY_W)) {
            m_Camera2D.target.y -= m_MoveSpeed;
        } else if (IsKeyDown(KEY_A)) {
            m_Camera2D.target.x -= m_MoveSpeed;
        } else if (IsKeyDown(KEY_S)) {
            m_Camera2D.target.y += m_MoveSpeed;
        } else if (IsKeyDown(KEY_D)) {
            m_Camera2D.target.x += m_MoveSpeed;
        } else {
            Vector2 deltaScale = Vector2Scale(delta, -1.0f/m_Camera2D.zoom);
            m_Camera2D.target = Vector2Add(m_Camera2D.target, deltaScale);
        }
    }

    void EditorCamera::ZoomCamera(const Vector2& delta, float wheel)
    {
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), m_Camera2D);
                
        m_Camera2D.offset = GetMousePosition();
        // Set the target to match, so that the camera maps the world space point
        // under the cursor to the screen space point under the cursor at any zoom
        m_Camera2D.target = mouseWorldPos;

        m_Camera2D.zoom += (wheel*m_ZoomSpeed);
        if (m_Camera2D.zoom < m_ZoomSpeed) { m_Camera2D.zoom = m_ZoomSpeed; }
    }
    
    void EditorCamera::OnUpdate(Timestep ts)
    {
        const Vector2 delta = GetMouseDelta();
        const float mouseWheel = GetMouseWheelMove();
        
        if (mouseWheel != 0) {
            ZoomCamera(delta, mouseWheel);
        }
        
        if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) { // support right mouse hold and wasd keyboard to move
            MoveCamera(delta);
        }
    }

}
