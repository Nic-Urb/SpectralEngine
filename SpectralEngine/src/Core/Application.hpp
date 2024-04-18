//
//  Application.hpp
//  SpectralEngine
//
//  Created by Nicolas U on 20.03.24.
//
#pragma once

#include "pch.h"
#include "Layer.hpp"
#include "LayerStack.hpp"
#include "Window.hpp"

namespace Spectral {
    
    class Application
    {
    public:
        
        Application(const std::string& name);
        virtual ~Application();
        
        void Run();
        
        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);
        
        static Application& GetInstance();
        
        Window& GetWindow() { return *m_Window; }
        
    private:
        std::unique_ptr<Window> m_Window;
        //ImGuiLayer* m_ImGuiLayer;
        LayerStack m_LayerStack;
    };

    Application* CreateApplication();
}
