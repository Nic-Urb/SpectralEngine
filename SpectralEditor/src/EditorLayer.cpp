//
//  EditorLayer.cpp
//  SpectralEditor
//
//  Created by Nicolas U on 04.04.24.
//

#include "EditorLayer.hpp"

#include "rlImGui.h"
#include "imgui.h"


EditorLayer::EditorLayer()
{
    // @TODO: Init all stuff OnAttach()
    Spectral::TextureManager::LoadTexture("ressources/Play.png");
    Spectral::TextureManager::LoadTexture("ressources/Stop.png");
    
    m_ActiveScene = std::make_shared<Spectral::Scene>();
    
    m_EditorCamera = Spectral::EditorCamera();
    m_HierarchyPanel= Spectral::HierarchyPanel(m_ActiveScene);
    
    m_Framebuffer = LoadRenderTexture(GetScreenWidth(), GetScreenHeight()); // @TODO: Change size
    SetTextureFilter(m_Framebuffer.texture, TEXTURE_FILTER_BILINEAR);
}

void EditorLayer::OnUpdate(Spectral::Timestep ts)
{
    if (m_CurrentState == SceneState::Edit)
    {
        m_EditorCamera.OnUpdate(ts);
    } else {
        m_ActiveScene->OnUpdateRuntime(ts);
    }
    
    // draw texture internally before the drawing phase
    BeginTextureMode(m_Framebuffer);
        ClearBackground(BLACK); // swap buffers
        if (m_CurrentState == SceneState::Edit)
        {
            m_ActiveScene->OnRenderEditor(m_EditorCamera);
        } else {
            m_ActiveScene->OnRenderRuntime();
        }
    EndTextureMode();
}

void EditorLayer::OnImGuiRender() 
{
    // create a docking space
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
    
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File")) {
            // OpenFile()
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Options")) {
            
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Tools")) {
            
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Save")) {
            // SaveFile()
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Help")) {
            
            ImGui::EndMenu();
        }
        
        DrawToolbar();
        
        // frame time counter
        ImGui::SameLine(ImGui::GetWindowWidth()-200.0f);
        ImGui::Text(" %i Client FPS (%.2f ms)", GetFPS(), GetFrameTime()*1000.0f);
        
        ImGui::EndMainMenuBar();
    }
    
    m_HierarchyPanel.OnImGuiRender();
    
    char buffer[128];
    std::string name = (m_CurrentState == SceneState::Edit) ? "Edit Mode" : "Play Mode";
    snprintf(buffer, sizeof(buffer), "Game (%s)###Viewport", name.c_str());
    ImGui::Begin(buffer);
        rlImGuiImageRenderTextureFit(&m_Framebuffer, true); // render framebuffer to imgui viewport
    ImGui::End();
    
    
    OnGizmoUpdate(); // update gizmo
}

void EditorLayer::DrawToolbar()
{
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    
    const ImVec4 greenColor = ImVec4(0.0f, 0.78f, 0.0f, 1.0f);
    const ImVec4 grayColor = ImVec4(0.78f, 0.78f, 0.78f, 1.0f);
    
    static ImVec4 playColor = greenColor;
    static ImVec4 stopColor =  grayColor;
    
    const Texture* playTexture = Spectral::TextureManager::GetTexture("ressources/Play.png").get();
    if (ImGui::ImageButton("##Play", (ImTextureID)playTexture, /*image size*/ImVec2(13.0f, 13.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(0.0f, 0.0f, 0.0f, 0.0f), playColor))
    {
        OnRuntimeStart();
        
        playColor = grayColor;
        stopColor = greenColor;
    }
    
    // @TODO: Add pause
    
    const Texture* stopTexture = Spectral::TextureManager::GetTexture("ressources/Stop.png").get();
    if (ImGui::ImageButton("##Stop", (ImTextureID)stopTexture, /*image size*/ImVec2(13.0f, 13.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(0.0f, 0.0f, 0.0f, 0.0f), stopColor))
    {
        OnRuntimeStop();
        
        playColor = greenColor;
        stopColor = grayColor;
    }
    
    ImGui::PopStyleColor(1);
}

void EditorLayer::OnGizmoUpdate()
{
    
}

void EditorLayer::OnRuntimeStart() // @TODO: Implement
{
    m_CurrentState = SceneState::Play;
    
    // m_ActiveScene = new Scene();
    // m_ActiveScene->OnRuntimeStart();
}

void EditorLayer::OnRuntimeStop() // @TODO: Implement
{
    m_CurrentState = SceneState::Edit;
    
    // m_ActiveScene = EditorScene;
    // m_ActiveScene->OnRuntimeStop();
}
