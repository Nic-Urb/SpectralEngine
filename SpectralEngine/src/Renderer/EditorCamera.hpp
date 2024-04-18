//
//  EditorCamera.hpp
//  SpectralEngine
//
//  Created by Nicolas U on 04.04.24.
//
#pragma once

#include "pch.h"
#include "Camera.h"
#include "Core/Timestep.h"
//#include "Objects/Object.hpp"

namespace Spectral {
    
    class EditorCamera : public Camera
    {
    public:
        EditorCamera();
        
        void OnUpdate(Timestep ts);
        
        Camera2D GetCamera() const { return m_Camera; }
        
    private:
        Camera2D m_Camera = { 0 };
        float m_MoveSpeed = 10.0f, m_ZoomSpeed = 0.125f;
        
    private:
        
        void CameraMove(const Vector2& delta); // @TODO: clamp value to map edges
        void CameraZoom(const Vector2& delta, float wheel); // @TODO: clamp value to map edges
        //void CenterCameraToObject(Object& object) {} // @TODO: implement this
        
    };
}
