//
//  EditorCamera.hpp
//  SpectralEngine
//
//  Created by Nicolas U on 04.04.24.
//
#pragma once

#include "pch.h"
#include "Camera.hpp"

namespace Spectral {
    
    class EditorCamera : public Camera
    {
    public:
        EditorCamera() = default;
        EditorCamera(const Camera3D camera);
        
        void OnUpdate(Timestep ts) override;
        
    private:
        float m_MoveSpeed = 10.0f, m_ZoomSpeed = 0.125f, m_PanSpeed = 0.003f; // @TODO: 3D: Use ZoomSpeed !
        
    private:
        void MoveCamera(const Vector2& delta); // @TODO: clamp values to map edges or some value
        void PanCamera(const Vector2& delta);
        
    };
}
