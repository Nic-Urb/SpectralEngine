//
//  ScriptGlue.cpp
//  SpectralEngine
//
//  Created by Nicolas U on 05.06.24.
//

#include "ScriptGlue.hpp"

#include "raylib.h"
#include "box2d/box2d.h"
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
    
    
        static void Physics_ApplyImpulse(RigidBody2DComponent& self, const Vector2& impulse, const Vector2& point, bool wake)
        {
            b2Body* body = (b2Body*)self.RuntimeBody;
            
            if (body->GetType() == b2_dynamicBody) {
                    body->ApplyLinearImpulse(b2Vec2(impulse.x, impulse.y), b2Vec2(point.x, point.y), wake);
            } else {
                SP_LOG_WARN("Physics_ApplyImpulse: Attempting to apply impulse to a non-dynamic body");
            }
        }
    }
    
    void ScriptGlue::RegisterMetaFunctions()
    {
        // register all component types
        RegisterMetaComponent<TransformComponent>();
        RegisterMetaComponent<RigidBody2DComponent>();
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
        
        lua.new_usertype<RigidBody2DComponent>(
                "RigidBody2D",
                sol::constructors<RigidBody2DComponent()>(),
                "type_id", &entt::type_hash<RigidBody2DComponent>::value,
                "isAwake", &RigidBody2DComponent::Awake,
                "gravityScale", &RigidBody2DComponent::GravityScale,
                "applyImpulse", &Physics_ApplyImpulse
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
        
        // math
        lua.new_usertype<Vector2>(
            "vector2",
            "x", &Vector2::x,
            "y", &Vector2::y
        );
        
        lua.new_usertype<Vector3>(
            "vector3",
            "x", &Vector3::x,
            "y", &Vector3::y,
            "z", &Vector3::z
        );
        
        lua.set_function("vector2", [](float x, float y) {
                return (Vector2){x, y};
            });
        
        lua.set_function("vector3", [](float x, float y) {
                return (Vector2){x, y};
            });
        
        // input functions
        lua.set_function("inputKeyPressed", IsKeyPressed);
        lua.set_function("inputKeyDown", IsKeyDown);
        
    }
}
