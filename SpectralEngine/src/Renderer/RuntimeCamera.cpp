//
//  RuntimeCamera.cpp
//  SpectralEngine
//
//  Created by Nicolas U on 21.04.24.
//

#include "RuntimeCamera.hpp"

namespace Spectral {

    RuntimeCamera::RuntimeCamera()
    {
        m_Camera2D.zoom = 1.0f;
    }

    void RuntimeCamera::OnUpdate(Timestep ts)
    {}

    void RuntimeCamera::FocusAt(const Vector2& position)
    {}
}
