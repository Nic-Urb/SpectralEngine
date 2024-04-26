//
//  EditorCamera.cpp
//  SpectralEngine
//
//  Created by Nicolas U on 04.04.24.
//

#include "EditorCamera.hpp"

#include "rlgl.h"
#include "raymath.h"
#include "rcamera.h"

namespace Spectral {

    EditorCamera::EditorCamera(const Camera3D camera) : Camera(camera)
    {
    }

    void EditorCamera::MoveCamera(const Vector2& delta)
    {
        // Keyboard support
        if (IsKeyDown(KEY_W)) {
            CameraMoveForward(&m_Camera3D, m_MoveSpeed, false);
        } else if (IsKeyDown(KEY_A)) {
            CameraMoveRight(&m_Camera3D, -m_MoveSpeed, false);
        } else if (IsKeyDown(KEY_S)) {
            CameraMoveForward(&m_Camera3D, -m_MoveSpeed, false);
        } else if (IsKeyDown(KEY_D)) {
            CameraMoveRight(&m_Camera3D, m_MoveSpeed, false);
        }
        
        // @TODO: Gamepad support
        /*if (IsGamepadAvailable(0)) {
        }*/
    }

    void EditorCamera::PanCamera(const Vector2& delta)
    {
        // Mouse support
        CameraYaw(&m_Camera3D, -delta.x*m_PanSpeed, false);
        CameraPitch(&m_Camera3D, -delta.y*m_PanSpeed, false, false, false);
        
        // @TODO: Gamepad support
        /*if (IsGamepadAvailable(0)) {
        }*/
    }
    
    void EditorCamera::OnUpdate(Timestep ts)
    {
        const Vector2 delta = GetMouseDelta();
        const float mouseWheel = GetMouseWheelMove();
        
        if (mouseWheel != 0) {
            CameraMoveToTarget(&m_Camera3D, mouseWheel);
        }
        
        if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) { // support right mouse hold and wasd keyboard to move
            PanCamera(delta);
            MoveCamera(delta);
        }
    }

}
