//
//  Window.hpp
//  SpectralEngine
//
//  Created by Nicolas U on 21.03.24.
//
#pragma once

#include "pch.h"

namespace Spectral {

    class Window
    {
    public:
        
        Window(const std::string& name, uint32_t width = 800, uint32_t height = 450);
        ~Window();
        
        void OnUpdate();
        
        uint32_t GetWidth() const { return m_ScreenWidth; }
        uint32_t GetHeight() const { return m_ScreenWidth; }
        
        void SetVSync(bool enabled);
        bool IsVSync() { return m_VSync; }
        
    private:
        
        void Init();
        
    private:
        
        std::string m_Name;// = "[Undefined window name - Application]"; // @TODO: Remove
        uint32_t m_ScreenWidth;// = 800;
        uint32_t m_ScreenHeight;// = 450;
        bool m_VSync;
        
    };
}
