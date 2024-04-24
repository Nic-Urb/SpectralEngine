//
//  EditorCamera.hpp
//  SpectralEngine
//
//  Created by Nicolas U on 04.04.24.
//
#pragma once

#include "pch.h"
#include "Camera.hpp"
//#include "Objects/Object.hpp"

namespace Spectral {
    
    class EditorCamera : public Camera
    {
    public:
        EditorCamera();
        
        void OnUpdate(Timestep ts) override;
        
    private:
        float m_MoveSpeed = 10.0f, m_ZoomSpeed = 0.125f;
        
    private:
        void MoveCamera(const Vector2& delta); // @TODO: clamp value to map edges or some value
        void ZoomCamera(const Vector2& delta, float wheel); // @TODO: clamp value to map edges or some value
        
    };
}
