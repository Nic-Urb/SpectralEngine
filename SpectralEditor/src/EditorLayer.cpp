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
    m_EditorScene = std::make_shared<Spectral::Scene>();
    
    m_EditorCamera = Spectral::EditorCamera();
    m_HierarchyPanel= Spectral::HierarchyPanel(m_EditorScene);
    
    m_Framebuffer = LoadRenderTexture(GetScreenWidth(), GetScreenHeight()); // @TODO: Change size
    SetTextureFilter(m_Framebuffer.texture, TEXTURE_FILTER_BILINEAR);
}

void EditorLayer::OnUpdate(Spectral::Timestep ts)
{
    m_EditorCamera.OnUpdate(ts);
    m_EditorScene->OnUpdateEditor(ts, m_EditorCamera);
    
    // draw texture internally before the drawing phase
    BeginTextureMode(m_Framebuffer);
        ClearBackground(BLACK); // swap buffers
        m_EditorScene->OnRenderEditor(m_EditorCamera);
    EndTextureMode();
}

void EditorLayer::OnImGuiRender() 
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Options"))
        {
            
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Tools"))
        {
            
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Save"))
        {
            
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Help"))
        {
            
            ImGui::EndMenu();
        }
        
        // frame time counter
        ImGui::SameLine(ImGui::GetWindowWidth()-200.0f);
        ImGui::Text(" %i Client FPS (%.2f ms)", GetFPS(), GetFrameTime()*1000.0f);
        
        
        ImGui::EndMainMenuBar();
    }
    
    m_HierarchyPanel.OnImGuiRender();
    
    ImGui::Begin("Viewport");
    rlImGuiImageRenderTextureFit(&m_Framebuffer, true);
    ImGui::End();
}
