project "imgui"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20" -- don't think we need this here
    staticruntime "off"

    targetdir "../../bin/%{cfg.platform}_%{cfg.buildcfg}/%{prj.name}"
    --objdir

    files {
        "imconfig.h",
		"imgui.h",
		"imgui.cpp",
		"imgui_draw.cpp",
		"imgui_internal.h",
		"imgui_widgets.cpp",
		"imstb_rectpack.h",
		"imstb_textedit.h",
		"imstb_truetype.h",
		"imgui_demo.cpp",
        "rlImGui.h",
        "rlImGui.cpp",
        "rlImGuiColors.h",
        "*.cpp",
        "*.h",
        "extras/**.h"
    }

    includedirs { --raylib
        "../raylib-master/src",
        "../raylib-master/src/external",
        "../raylib-master/src/external/glfw/include" 
    }
    
    defines {
        "IMGUI_DISABLE_OBSOLETE_FUNCTIONS",
        "IMGUI_DISABLE_OBSOLETE_KEYIO"
    }
