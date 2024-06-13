project "JoltPhysics"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir "%{wks.location}/bin/%{cfg.platform}_%{cfg.buildcfg}/%{prj.name}"

	files {
		"Jolt/**.h",
		"Jolt/**.cpp"
	}

	sysincludedirs {
		"."
	}
