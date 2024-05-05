
project "SpectralEngine"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "on" -- why using runtime on ??

    targetdir "%{wks.location}/bin/%{cfg.platform}_%{cfg.buildcfg}/%{prj.name}"
    --objdir "../bin-int/%{cfg.platform}_%{cfg.buildcfg}"

    files {
        "src/**.c", 
        "src/**.cpp", 
        "src/**.h", 
        "src/**.hpp",
        "vendor/ImGuizmo/ImGuizmo.cpp",
        "vendor/ImGuizmo/ImGuizmo.h",
        "vendor/tinyfiledialogs/tinyfiledialogs.cpp",
        "vendor/tinyfiledialogs/tinyfiledialogs.h"
    }
  
    includedirs { 
        "src",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.imgui}",
        "%{IncludeDir.yaml_cpp}",
        "%{IncludeDir.imguizmo}"
    }

    --filter "files::vendor/ImGuizmo/**.cpp"
      --  flags {"NoPCH"}

    --link_raylib()
    include_raylib()
    -- if we need raylib in this library, then change to link !

    filter "action:xcode4"
        --pchheader ""
        --pchsource ""
        
        -- this is required by xcode, means that the header files enclosed in angle brackets
        -- will search System Header Search Paths and Header Search Paths
        -- includedirs append specified directory to User Header Search Paths, but XCode does
        -- not search for header files enclosed in angle brackets in the User Header Search Paths
        xcodebuildsettings = { ["ALWAYS_SEARCH_USER_PATHS"] = "YES" }
        externalincludedirs {
            "%{IncludeDir.spdlog}"
        }

    filter {}

    links {
        "imgui",
        "yaml-cpp"
    }