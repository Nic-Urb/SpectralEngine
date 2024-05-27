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

#include "../../SpectralEditor/ressources/fonts/opensans-main/Regular.cpp"
#include "../../SpectralEditor/ressources/fonts/materialdesign-main/IconsMaterialDesign.h"
#include "../../SpectralEditor/ressources/fonts/materialdesign-main/IconsMaterialDesign.cpp"

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
        SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_UNDECORATED);
        
        InitWindow(m_ScreenWidth, m_ScreenHeight, m_Name.c_str());
        SetTargetFPS(60);
        SP_LOG_INFO("ENGINE::Creating window ({0}, {1})", m_ScreenWidth, m_ScreenHeight);
        
        // setup imgui // @TODO: Push as overlay to layer stack
        IMGUI_CHECKVERSION();
        rlImGuiSetup(true);
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        
        // font
        ImFontConfig fontCfg; // change font cfg setting for higher dpi
        fontCfg.FontDataOwnedByAtlas = false;
        fontCfg.OversampleH = 2;
        fontCfg.OversampleV = 2;
        fontCfg.RasterizerMultiply = 1.5f;
        io.FontDefault = io.Fonts->AddFontFromMemoryCompressedTTF(Font_Regular_compressed_data, Font_Regular_compressed_size, 17.0f, &fontCfg);
        
        
        // icon font
        static const ImWchar icons_ranges[] = {ICON_MIN_MD, ICON_MAX_16_MD, 0};
        ImFontConfig iconCfg;
        iconCfg.MergeMode = true;
        iconCfg.PixelSnapH = true;
        iconCfg.GlyphMinAdvanceX = 20.0f;
        iconCfg.GlyphOffset.y = 4.0f;
        iconCfg.OversampleH = 2;
        iconCfg.OversampleV = 2;
        fontCfg.RasterizerMultiply = 1.5f;
        io.Fonts->AddFontFromMemoryCompressedTTF(MaterialIcons_compressed_data, MaterialIcons_compressed_size, 20.0f, &iconCfg, icons_ranges);
        
        rlImGuiReloadFonts();
        
        // style
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 5.0f;
        style.FrameRounding = 5.0f;
        style.WindowBorderSize = 0.0f;
    }

    void Window::OnUpdate() 
    {
    }

    void Window::SetVSync(bool enabled)
    {
        // use gl for setting vsync off/on
    }
}


