//
//  MetaHelper.hpp
//  SpectralEngine
//
//  Created by Nicolas U on 08.06.24.
//
#pragma once

#include "entt.hpp"
#include "sol.hpp"

namespace Spectral {
    
    // https://github.com/skaarj1989/entt-meets-sol2/blob/main/utility/meta_helper.hpp
    
    [[nodiscard]] entt::id_type get_type_id(const sol::table &obj)
    {
        const auto f = obj["type_id"].get<sol::function>();
        assert(f.valid() && "type_id not exposed to lua!");
        return f.valid() ? f().get<entt::id_type>() : -1;
    }
    
    template <typename T>
    [[nodiscard]] entt::id_type deduce_type(T &&obj)
    {
        switch (obj.get_type()) {
                // in lua: entity:has(Transform.type_id())
            case sol::type::number:
                return obj.template as<entt::id_type>();
                // in lua: entity:has(Transform)
            case sol::type::table:
                return get_type_id(obj);
        }
        assert(false);
        return -1;
    }
    
    // @see
    // https://github.com/skypjack/entt/wiki/Crash-Course:-runtime-reflection-system
    
    template <typename... Args>
    inline auto invoke_meta_func(entt::meta_type meta_type,
                                entt::id_type function_id, Args &&...args)
    {
        if (!meta_type) {
            // @TODO: Handle error
        } else {
            if (auto &&meta_function = meta_type.func(function_id); meta_function)
                return meta_function.invoke({}, std::forward<Args>(args)...);
        }
        return entt::meta_any{};
    }
    
    template <typename... Args>
    inline auto invoke_meta_func(entt::id_type type_id, entt::id_type function_id,
                                Args &&...args)
    {
        return invoke_meta_func(entt::resolve(type_id), function_id,
                                std::forward<Args>(args)...);
    }
}
