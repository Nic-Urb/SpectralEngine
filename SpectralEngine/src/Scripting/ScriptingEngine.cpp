//
//  ScriptingEngine.cpp
//  SpectralEngine
//
//  Created by Nicolas U on 04.06.24.
//

#include "ScriptingEngine.hpp"

#include "lua.hpp"
#include "ScriptGlue.hpp"

#include <filesystem>

namespace Spectral {

    static sol::state s_LuaState;

    // @TODO: Handle panic
    static void my_panic(sol::optional<std::string> maybe_msg) {
        std::cerr << "Lua is in a panic state and will now destroy the Lua state." << std::endl;
        if (maybe_msg) {
            const std::string& msg = maybe_msg.value();
            std::cerr << msg << std::endl;
        }
        
        // Clean up the Lua state safely
        lua_close(s_LuaState);
    }

    static sol::load_result LoadScript(const std::string& path)
    {
        auto loadedResult = s_LuaState.load_file(path);
        
        if (!loadedResult.valid())
        {
            sol::error error = loadedResult;
            SP_LOG_ERORR("ScriptingEngine::LoadScript - ({0})", error.what());
        }
        else
        {
            SP_LOG_INFO("ScriptingEngine::LoadScript - Script loaded successfully");
        }
        return loadedResult;
    }

    
    void ScriptingEngine::Init()
    {
        SP_LOG_INFO("ScriptingEngine::Init");
        
        ScriptGlue::RegisterMetaFunctions();
        
        s_LuaState = sol::state(sol::c_call<decltype(&my_panic), &my_panic>);
        
        s_LuaState.open_libraries(sol::lib::base, sol::lib::math, sol::lib::package);
        
        ScriptGlue::RegisterFunctions(s_LuaState);
        ScriptGlue::RegisterComponents(s_LuaState);
        
        // temporary path for packages, like keyboard_keys
        std::string packagePath = "assets/lua_package/";
        if (std::filesystem::exists(packagePath))
        {
            SP_LOG_INFO("ScriptingEngine::Init - package path ({0}) found", packagePath);
            s_LuaState["package"]["path"] = s_LuaState["package"]["path"].get<std::string>() + ";" + packagePath + "?.lua";
        }
    }

    void ScriptingEngine::OnCreate(Entity entity)
    {
        auto& lsc = entity.GetComponent<LuaScriptComponent>();
        
        if (!lsc.ScriptPath.empty())
        {
            auto loaded_result = LoadScript(lsc.ScriptPath);
            
            lsc.self = loaded_result.call();
            
            if (!lsc.self.valid())
            {
                // @TODO: Log error
                return;
            }
            
            lsc.self["owner"] = std::ref(entity); // make the owner entity available to lua
            
            sol::protected_function create = lsc.self["OnCreate"];
            
            if (create.valid())
            {
                // execute script function
                sol::protected_function_result result = create(lsc.self);
                
                if (!result.valid())
                {
                    sol::error error = result;
                    SP_LOG_ERORR("ScriptingEngine::OnCreate - ({0})", error.what());
                }
            }
        }
    }

    void ScriptingEngine::OnUpdate(Entity entity, float ts)
    {
        auto& lsc = entity.GetComponent<LuaScriptComponent>();
        
        if (!lsc.ScriptPath.empty())
        {
            lsc.self["owner"] = std::ref(entity); // make the owner entity available to lua
            
            sol::protected_function update = lsc.self["OnUpdate"];
            if (update.valid())
            {
                // execute script function
                sol::protected_function_result result = update(lsc.self,ts);
                if (!result.valid())
                {
                    sol::error error = result;
                    SP_LOG_ERORR("ScriptingEngine::OnUpdate - ({0})", error.what());
                }
            }
        }
    }

    // @TODO: OnIdle() -> lua.collect_garbage();
}
