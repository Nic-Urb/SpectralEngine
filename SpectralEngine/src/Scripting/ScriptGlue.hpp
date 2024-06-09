//
//  ScriptGlue.hpp
//  SpectralEngine
//
//  Created by Nicolas U on 05.06.24.
//
#pragma once

#include "sol.hpp"

namespace Spectral {

    // Register binding between C++ and Lua
    class ScriptGlue
    {
    public:
        static void RegisterMetaFunctions();
        static void RegisterComponents(sol::state& lua);
        static void RegisterFunctions(sol::state& lua);
    };
}
