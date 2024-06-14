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

#include "Renderer/Shaders.hpp"
#include "Scripting/ScriptingEngine.hpp"


namespace Spectral {

    Application* Application::s_Instance = nullptr;

    Application::Application()
    {
        s_Instance = this;
        m_Window = std::make_unique<Window>();
        
        // m_ImGuiLayer = new ImGuiLayer();
        // PushOverlay(m_ImGuiLayer);
        ScriptingEngine::Init();
        Shaders::LoadShaders();
    }

    Application::~Application()
    {
        SP_LOG_INFO("Engine::Shutdown");
        Shaders::UnloadShaders();
    }

    void Application::Run()
    {
        SP_LOG_INFO("Engine::Run");
        float lastFrameTime = 0.0f;
        
        // engine main loop
        while (!WindowShouldClose())
        {
            float time = (float)GetTime();
            m_Timestep = time - lastFrameTime;
            
            lastFrameTime = time;
            
            for (Layer* layer : m_LayerStack) {
                layer->OnUpdate(m_Timestep);
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
    }

    void Application::PushLayer(Layer* layer)
    {
        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PushOverlay(Layer* overlay)
    {
        m_LayerStack.PushOverlay(overlay);
        overlay->OnAttach();
    }

}
