
newoption
{
    trigger = "graphics",
    value = "OPENGL_VERSION",
    description = "version of OpenGL to build raylib against",
    allowed = {
        { "opengl11", "OpenGL 1.1"},
        { "opengl21", "OpenGL 2.1"},
        { "opengl33", "OpenGL 3.3"},
        { "opengl43", "OpenGL 4.3"}
    },
    default = "opengl33"
}

workspace ("Spectral")
    --platforms { "x64", "x86", "Arm64"}
    --defaultplatform ("Arm64")
    architecture "ARM64"

    --filter { "platforms:x64" }
    --    architecture "x86_64"

    --filter { "platforms:Arm64" }
      --  architecture "ARM64"

    --filter {}

    startproject ("SpectralEditor")

    configurations {
        "Debug", 
        "Release"
    }

targetdir "bin/%{cfg.buildcfg}/"

IncludeDir = {}

IncludeDir["spdlog"] = "%{wks.location}/SpectralEngine/vendor/spdlog/include"
IncludeDir["imgui"] = "%{wks.location}/SpectralEngine/vendor/imgui-master"
IncludeDir["yaml_cpp"] = "%{wks.location}/SpectralEngine/vendor/yaml-cpp/include"
--box2d

group "Dependencies"
    include ("raylib_premake5.lua")
    include ("SpectralEngine/vendor/imgui-master")
    include ("SpectralEngine/vendor/yaml-cpp")

group "Core"
    include ("SpectralEngine")
    --include ("SpectralScript") 
    --include ("Application/main")

group "Tools"
    include ("SpectralEditor")
    include ("Sandbox")



--- look at this code on left side an mimic them ---