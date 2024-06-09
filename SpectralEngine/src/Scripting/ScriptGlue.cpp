//
//  ScriptGlue.cpp
//  SpectralEngine
//
//  Created by Nicolas U on 05.06.24.
//

#include "ScriptGlue.hpp"

#include "raylib.h"
#include "MetaHelper.hpp"
#include "Entt/Entity.hpp"
#include "Entt/Components.hpp"


namespace Spectral {
    
    // wrap the static functions inside an unnamed namespace to limits their scope to the file in which they are declared
    namespace {
        template <typename T>
        static auto Entity_GetComponent(Entity* entity, sol::this_state s)  // entity needs to be passed by pointer
        {
            auto& component = entity->GetComponent<T>();
            return sol::make_reference(s, std::ref(component));
        }
        
        template <typename T>
        static bool Entity_HasComponent(Entity* entity)
        {
            return entity->HasComponent<T>();
        }
        
        template <typename T>
        static bool Entity_RemoveComponent(Entity* entity) {
            return entity->RemoveComponent<T>();
        }
        
        template <typename T>
        static void RegisterMetaComponent()
        {
            using namespace entt::literals;
            
            entt::meta<T>()
                .template func<&Entity_HasComponent<T>>("has"_hs)
                .template func<&Entity_GetComponent<T>>("get"_hs)
                .template func<&Entity_RemoveComponent<T>>("remove"_hs)
            ;
        }
    }
    
    void ScriptGlue::RegisterMetaFunctions()
    {
        // register all component types
        RegisterMetaComponent<TransformComponent>();
        // add additional components here as needed
    }
    
    
    void ScriptGlue::RegisterComponents(sol::state& lua)
    {
        lua.new_usertype<TransformComponent>(
                "Transform",
                sol::constructors<TransformComponent()>(),
                "type_id", &entt::type_hash<TransformComponent>::value, // expose type_id to LUA
                "translation", &TransformComponent::Translation,
                "rotation", &TransformComponent::Rotation,
                "scale", &TransformComponent::Scale
                );
        
    }

    void ScriptGlue::RegisterFunctions(sol::state& lua)
    {
        using namespace entt::literals;
        
        // register the Entity class and its methods (e.g. has, get, remove) with sol so that Lua can call these methods on an entity object
        lua.new_usertype<Entity>(
                "Entity",
                sol::constructors<Entity()>(),
                                 
                "has", [](Entity &self, const sol::object &type_or_id) {
                        const auto maybe_any =
                        invoke_meta_func(deduce_type(type_or_id), "has"_hs, &self);
                    
                        return maybe_any ? maybe_any.cast<bool>() : false;
                        },
                                 
                "get", [](Entity &self, const sol::object &type_or_id, sol::this_state s) {
                    const auto maybe_any =
                    invoke_meta_func(deduce_type(type_or_id), "get"_hs, &self, s);
                    
                    return maybe_any ? maybe_any.cast<sol::reference>() : sol::lua_nil_t{};
                },
                                 
                "remove", [](Entity &self, const sol::object &type_or_id) {
                    const auto maybe_any =
                    invoke_meta_func(deduce_type(type_or_id), "remove"_hs, &self);
                    
                    return maybe_any? maybe_any.cast<bool>() : false;
                }
                                 
                );
        
        // math functions
        lua.new_usertype<Vector3>(
            "vector3",
            sol::constructors<Vector3()>(),
            "x", &Vector3::x,
            "y", &Vector3::y,
            "z", &Vector3::z
        );
        
        // @TODO: physics functions
        
        // input functions
        lua.set_function("inputKeyPressed", IsKeyPressed);
        lua.set_function("inputKeyDown", IsKeyDown);
        
    }
}
