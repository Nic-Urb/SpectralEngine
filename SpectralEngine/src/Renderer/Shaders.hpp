//
//  Shaders.hpp
//  SpectralEngine
//
//  Created by Nicolas U on 14.06.24.
//
#pragma once

#include "raylib.h"

namespace Spectral {
    
    // @NOTE: this is a temporary solution, normally we would precompile all the shaders and cache them for GPU
    class Shaders
    {
    public:
        static void LoadShaders();
        static void UnloadShaders();
        
        // @NOTE: For now we're only using lighting shader
        static const Shader& GetLightingShader() { return s_LightingShader; }
        
    private:
        static Shader s_LightingShader;
    };
}
