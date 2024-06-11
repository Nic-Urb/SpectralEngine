project "box2d"
    kind "StaticLib"
	language "C++"
	cppdialect "C++11"
	staticruntime "off"

    targetdir "%{wks.location}/bin/%{cfg.platform}_%{cfg.buildcfg}/%{prj.name}"

    files {
		"src/**.h",
		"src/**.cpp",
		"include/**.h"
	}

	includedirs {
		"include",
		"src"
	}