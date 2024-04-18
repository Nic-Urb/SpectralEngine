project "yaml-cpp"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"

    targetdir "../../bin/%{cfg.platform}_%{cfg.buildcfg}/%{prj.name}"
    --objdir

    files {
        "src/**.h",
        "src/**.cpp",
        "include/**.h"
    }

    includedirs {
        "include"
    }