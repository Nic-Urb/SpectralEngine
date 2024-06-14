//
//  Shaders.cpp
//  SpectralEngine
//
//  Created by Nicolas U on 14.06.24.
//

#include "Shaders.hpp"

#include "Core/Log.hpp"

namespace Spectral {

    Shader Shaders::s_LightingShader;

    void Shaders::LoadShaders()
    {
        SP_LOG_INFO("Shaders::LoadingShaders");
        
        s_LightingShader = ::LoadShader("ressources/shaders/lighting.fs", "ressources/shaders/lighting.fs");
    }

    void Shaders::UnloadShaders()
    {
        SP_LOG_INFO("Shaders::UnloadingShaders");
        
        ::UnloadShader(s_LightingShader);
    }
}
