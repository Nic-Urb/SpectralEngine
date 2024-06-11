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
        
        const Matrix& GetTransform() { return m_StoredTransform; }
        void SetTransform(const Matrix& transform) { m_StoredTransform = transform; }
        
    private:
        // @TODO: temp solution
        Matrix m_StoredTransform;
        
        // ...
    };
}
