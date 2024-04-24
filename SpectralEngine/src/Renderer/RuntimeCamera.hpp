//
//  RuntimeCamera.hpp
//  SpectralEngine
//
//  Created by Nicolas U on 21.04.24.
//
#pragma once

#include "pch.h"

#include "Camera.hpp"

namespace Spectral {
    class RuntimeCamera : public Camera
    {
    public:
        RuntimeCamera();
        
        void OnUpdate(Timestep ts) override;
        
        void FocusAt(const Vector2& position);
        
    private:
        
        // ...
    };
}
