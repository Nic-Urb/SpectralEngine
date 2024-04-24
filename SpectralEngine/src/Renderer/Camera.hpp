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
        Camera(Camera2D& camera);
        virtual ~Camera() = default;
        
        virtual void OnUpdate(Timestep ts) = 0;
        
        // shared functions
        void BlendCameras(Camera2D& otherCamera, float delta);
        Camera2D GetCamera2D() const { return m_Camera2D; }
        
    protected:
        Camera2D m_Camera2D = (Camera2D){ 0 };
        
    protected:
        // focus camera on some position
        virtual void FocusCamera(const Vector2& position, float width, float height);

    };
}
