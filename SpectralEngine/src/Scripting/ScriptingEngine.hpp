//
//  ScriptingEngine.hpp
//  SpectralEngine
//
//  Created by Nicolas U on 04.06.24.
//

#include "pch.h"

#include "sol.hpp"
#include "Entt/Entity.hpp"

namespace Spectral {

    class ScriptingEngine
    {
    public:
        static void Init();
        
        static void OnCreate(Entity entity);
        static void OnUpdate(Entity entity, float ts);
    };
}
