//
//  Log.cpp
//  SpectralEngine
//
//  Created by Nicolas U on 19.03.24.
//

#include "Log.hpp"

namespace Spectral {

    std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
    std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

    void Log::init()
    {
        spdlog::set_pattern("%^[%T] %n: %v%$");
        
        s_CoreLogger = spdlog::stdout_color_mt("ENGINE");
        s_CoreLogger->set_level(spdlog::level::trace);
        
        s_ClientLogger = spdlog::stdout_color_mt("CLIENT");
        s_ClientLogger->set_level(spdlog::level::trace);
    }
    
}

