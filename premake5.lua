
newoption
{
    trigger = "graphics",
    value = "OPENGL_VERSION",
    description = "version of OpenGL to build raylib against",
    allowed = {
        { "opengl11", "OpenGL 1.1"},
        { "opengl21", "OpenGL 2.1"},
        { "opengl33", "OpenGL 3.3"},
        { "opengl43", "OpenGL 4.3"},
        { "opengles2", "OpenGLES 2.0"},
        { "opengles3", "OpenGLES 3.0"}
    },
    default = "opengles2"
}

-- checks and copy ANGLE dylibs from Chrome
function check_angle()
    local chrome_path = "/Applications/Google Chrome.app/Contents/Frameworks/Google Chrome Framework.framework/Versions/123.0.6312.107/Libraries/"
    if(os.isdir(chrome_path)) then
        print("Initializing ANGLE ...")

        local angle_dir = "SpectralEngine/vendor/chrome-angle"
        if not os.isdir(angle_dir) then
            os.mkdir(angle_dir)
        else
            print("Directory " .. angle_dir .. " already exists, skipping creation")
        end

        -- Copy ANGLE dylibs from Google Chrome
        os.copyfile(chrome_path .. "libEGL.dylib", angle_dir)
        os.copyfile(chrome_path .. "libGLESv2.dylib", angle_dir)

        -- Change the rpath using install_name_tool (invalidates signature)
        os.execute('install_name_tool -id "@rpath/libEGL.dylib" "' .. angle_dir .. '/libEGL.dylib"')
        os.execute('install_name_tool -id "@rpath/libGLESv2.dylib" "' .. angle_dir .. '/libGLESv2.dylib"')

        -- Remove the signature
        os.execute('codesign -s "-" -fv "' .. angle_dir .. '/libEGL.dylib"')
        os.execute('codesign -s "-" -fv "' .. angle_dir .. '/libGLESv2.dylib"')
    else 
        print("Can't find Google Chrome directory, make sure Chrome is installed")
    end
end


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

check_angle()

IncludeDir = {}

IncludeDir["spdlog"] = "%{wks.location}/SpectralEngine/vendor/spdlog/include"
IncludeDir["imgui"] = "%{wks.location}/SpectralEngine/vendor/imgui-master"
IncludeDir["yaml_cpp"] = "%{wks.location}/SpectralEngine/vendor/yaml-cpp/include"
IncludeDir["imguizmo"] = "%{wks.location}/SpectralEngine/vendor/ImGuizmo"
IncludeDir["tinyfiledialogs"] = "%{wks.location}/SpectralEngine/vendor/tinyfiledialogs"

group "Dependencies"
    include ("raylib_premake5.lua")
    include ("SpectralEngine/vendor/imgui-master")
    include ("SpectralEngine/vendor/yaml-cpp")

group "Core"
    include ("SpectralEngine")

group "Tools"
    include ("SpectralEditor")
    include ("Sandbox")
