//
//  Window.cpp
//  SpectralEngine
//
//  Created by Nicolas U on 21.03.24.
//

#include "Window.hpp"

#include "raylib.h"
#include "rlImGui.h"
#include "imgui.h"


namespace Spectral {

    Window::Window(const std::string& name, uint32_t width, uint32_t height) : m_Name(name), m_ScreenWidth(width), m_ScreenHeight(height)
    {
        Init();
        
        if (!IsWindowReady()) {
            SP_LOG_ERORR("ENGINE::Window (Can't initialize window)");
        }
    }

    Window::~Window()
    {
        rlImGuiShutdown();
        
        CloseWindow();
    }

    void Window::Init()
    {
        SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT /*| FLAG_WINDOW_UNDECORATED*/);
        InitWindow(m_ScreenWidth, m_ScreenHeight, m_Name.c_str());
        SetTargetFPS(60);
        SP_LOG_INFO("ENGINE::Creating window ({0}, {1})", m_ScreenWidth, m_ScreenHeight);
        
        // setup imgui // @TODO: Push as overlay to layer stack
        rlImGuiSetup(false);
        ImGuiIO& io = ImGui::GetIO();
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        // @TODO: Implement docking support, use docking branch (https://github.com/ocornut/imgui/tree/docking)
        
        // fonts
        /*float fontSize = 18.0f;// *2.0f;
        io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Bold.ttf", fontSize);
        io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Regular.ttf", fontSize);*/
        
        // style
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 5.0f;
    }

    void Window::OnUpdate() 
    {
    }

    void Window::SetVSync(bool enabled)
    {
        // use gl for setting vsync off/on
        
    }
}


