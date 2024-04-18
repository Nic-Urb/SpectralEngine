//
//  Log.hpp
//  SpectralEngine
//
//  Created by Nicolas U on 19.03.24.
//
#pragma once

#include "pch.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Spectral {
    class Log
    {
    public:
        
        static void init();
        
        static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
        static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
        
    private:
        
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;
    };
}

// Core logging macros
#define SP_LOG_TRACE(...)  ::Spectral::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define SP_LOG_INFO(...)   ::Spectral::Log::GetCoreLogger()->info(__VA_ARGS__)
#define SP_LOG_WARN(...)   ::Spectral::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define SP_LOG_ERORR(...)  ::Spectral::Log::GetCoreLogger()->error(__VA_ARGS__)
#define SP_LOG_FATAL(...)  ::Spectral::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// Client logging macros
#define SP_CLIENT_LOG_TRACE(...)  ::Spectral::Log::GetClientLogger()->trace(__VA_ARGS__)
#define SP_CLIENT_LOG_INFO(...)   ::Spectral::Log::GetClientLogger()->info(__VA_ARGS__)
#define SP_CLIENT_LOG_WARN(...)   ::Spectral::Log::GetClientLogger()->warn(__VA_ARGS__)
#define SP_CLIENT_LOG_ERORR(...)  ::Spectral::Log::GetClientLogger()->error(__VA_ARGS__)
#define SP_CLIENT_LOG_FATAL(...)  ::Spectral::Log::GetClientLogger()->fatal(__VA_ARGS__)


