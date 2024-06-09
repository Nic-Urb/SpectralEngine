project "lua"
    kind "StaticLib"
    language "C++"
    staticruntime "on"

    targetdir "../../bin/%{cfg.platform}_%{cfg.buildcfg}/%{prj.name}"
    --objdir

    filter "configurations:Debug"
        defines "LUA_USE_APICHECK=1" 

    filter{}

    files {
        "src/**.h",
        "src/**.c",
        "sol2/include/**.hpp"
    }

    excludes {
        "src/luac.c"
    }

    includedirs {
        "sol2/include",
        "src"
    }

    