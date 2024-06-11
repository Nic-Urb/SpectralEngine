//
//  Renderer.hpp
//  SpectralEngine
//
//  Created by Nicolas U on 02.06.24.
//

#include "pch.h"

#include "raylib.h"

#include "RuntimeCamera.hpp"

namespace Spectral {
    
    class Renderer
    {
    public:
        
        static void RenderTexturedPlane(const Texture &texture, const Matrix &transform, const Vector4 &tint);
        static void RenderCameraDebugLines(std::shared_ptr<RuntimeCamera> camera, Color color);
    };
}
