//
//  Application.cpp
//  SpectralEngine
//
//  Created by Nicolas U on 20.03.24.
//

#include "Application.hpp"

#include "Timestep.h"
#include "rlImGui.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "raylib.h"


namespace Spectral {

    Application::Application(const std::string& name)
    {
        m_Window = std::make_unique<Window>(name);
        
        // m_ImGuiLayer = new ImGuiLayer();
        // PushOverlay(m_ImGuiLayer);
        // ScriptEngine::Init();
    }

    Application::~Application()
    {
        SP_LOG_INFO("Engine::Shutdown");
        //ScriptEngine::Shutdown();
    }

    Application& Application::GetInstance()
    {
        static Application* instance;
        return *instance;
    }

    void Application::Run()
    {
        SP_LOG_INFO("Engine::Run");
        float lastFrameTime = 0.0f;
        
        // engine main loop
        while (!WindowShouldClose())
        {
            float time = (float)GetTime();
            float timestep = time - lastFrameTime;
            
            lastFrameTime = time;
            
            for (Layer* layer : m_LayerStack) {
                layer->OnUpdate(timestep);
            }

            BeginDrawing();
                ClearBackground(BLACK); // swap buffers
            
                    for (Layer* layer : m_LayerStack) {
                            layer->OnRender();
                    }

                rlImGuiBegin();
                ImGuizmo::BeginFrame();
            
                    for (Layer* layer : m_LayerStack) {
                        layer->OnImGuiRender(); }
            
                rlImGuiEnd();

            EndDrawing();
        }
        
        //m_Window->OnUpdate();
    }

    void Application::PushLayer(Layer* layer)
    {
        m_LayerStack.PushLayer(layer);
    }

    void Application::PushOverlay(Layer* overlay)
    {
        m_LayerStack.PushOverlay(overlay);
    }

}
