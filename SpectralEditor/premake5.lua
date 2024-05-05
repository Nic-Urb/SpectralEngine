
project "SpectralEditor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir "%{wks.location}/bin/%{cfg.platform}_%{cfg.buildcfg}/%{prj.name}"
    --objdir "../bin-int/%{cfg.platform}_%{cfg.buildcfg}"

    files {
        "src/**.c", 
        "src/**.cpp", 
        "src/**.h", 
        "src/**.hpp",
    }
  
    includedirs { 
        "src",
        "%{wks.location}/SpectralEngine/src",
        "%{wks.location}/SpectralEngine/vendor",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.imgui}",
        "%{IncludeDir.yaml_cpp}",
        "%{IncludeDir.imguizmo}",
        "%{IncludeDir.tinyfiledialogs}"
    }

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
    
    link_raylib()

    links {
        "SpectralEngine"
        --"yaml-cpp"
        --"imgui"
    }