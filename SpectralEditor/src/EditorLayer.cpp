//
//  EditorLayer.cpp
//  SpectralEditor
//
//  Created by Nicolas U on 04.04.24.
//

#include "EditorLayer.hpp"

#include "Core/SceneSerializer.hpp"

#include "rlImGui.h"
#include "imgui.h"
#include "raymath.h"
#include "ImGuizmo.h"
#include "tinyfiledialogs.h"

#include <filesystem>


EditorLayer::EditorLayer()
{
    // @TODO: Init all stuff OnAttach()
    Spectral::TextureManager::LoadTexture("ressources/Play.png");
    Spectral::TextureManager::LoadTexture("ressources/Stop.png");
    
    m_ActiveScene = std::make_shared<Spectral::Scene>("TestScene");
    
    m_EditorCamera = Spectral::EditorCamera();
    m_HierarchyPanel= Spectral::HierarchyPanel(m_ActiveScene);
    m_ContentBrowserPanel = ContentBrowserPanel();
    
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
    
    // handle input events for shortcuts
    if (IsKeyPressed(KEY_Q)) {
        m_CurrentGizmo = -1;
    }
    if (IsKeyPressed(KEY_T)) {
        m_CurrentGizmo = ImGuizmo::OPERATION::TRANSLATE;
    }
    if (IsKeyPressed(KEY_E)) {
        m_CurrentGizmo = ImGuizmo::OPERATION::SCALE;
    }
    if (IsKeyPressed(KEY_R)) {
        m_CurrentGizmo = ImGuizmo::OPERATION::ROTATE;
    }
    
    // multiple keys events for shortcuts
    bool shift = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
    bool command = IsKeyDown(KEY_LEFT_SUPER) || IsKeyDown(KEY_RIGHT_SUPER);
    
    if (command && IsKeyPressed(KEY_S)) {
        QuickSave();
    }
    if (command && IsKeyPressed(KEY_O)) {
        OpenFile();
    }
    if (shift && command && IsKeyPressed(KEY_S)) {
        SaveFile();
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
        if (ImGui::BeginMenu("File")) 
        {
            if (ImGui::MenuItem("Save","Command+S")) {
                QuickSave();
            }
            
            if (ImGui::MenuItem("Save as...","Shift+Command+S")) {
                SaveFile();
            }
            
            if (ImGui::MenuItem("Open...", "Command+O")) {
                OpenFile();
            }
            
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
        
        if (ImGui::BeginMenu("Help")) 
        {
            
            ImGui::EndMenu();
        }
        
        DrawToolbar();
        
        // frame time counter
        ImGui::SameLine(ImGui::GetWindowWidth()-200.0f);
        ImGui::Text(" %i Client FPS (%.2f ms)", GetFPS(), GetFrameTime()*1000.0f);
        
        ImGui::EndMainMenuBar();
    }
    
    m_HierarchyPanel.OnImGuiRender();
    m_ContentBrowserPanel.OnImGuiRender();
    
    char buffer[128];
    std::string name = (m_CurrentState == SceneState::Edit) ? "Edit Mode" : "Play Mode";
    snprintf(buffer, sizeof(buffer), "%s (%s)###Viewport",m_ActiveScene->GetName().c_str() ,name.c_str());
    ImGui::Begin(buffer);
        rlImGuiImageRenderTextureFit(&m_Framebuffer, true); // render framebuffer to imgui viewport
        
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_PAYLOAD"))
            {
                const char* path = (const char*)payload->Data;
                LoadFile(path);
            }
            
            ImGui::EndDragDropTarget();
        }
    
        if (m_CurrentState == SceneState::Edit) {
            OnGizmoUpdate(); // updates and render gizmo only in editor mode
        }
    ImGui::End();
    
    if (ImGui::Begin("Engine Stats")) { // @TODO: Do as seperate panel class
        ImGuiIO& io = ImGui::GetIO();
        if (ImGui::IsMousePosValid())
        {
            ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
        }
    }
    ImGui::End();
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
    Spectral::Object* selectedObject = m_HierarchyPanel.GetSelectedContext();
    if (selectedObject && m_CurrentGizmo != -1)
    {
        Spectral::TransformComponent* tc = selectedObject->GetComponent<Spectral::TransformComponent>();
        
        if (!tc) {
            return;
        }
        
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        
        float windowWidth = (float)ImGui::GetWindowWidth();
        float windowHeight = (float)ImGui::GetWindowHeight();
        
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
        
        Matrix cameraProjection = m_EditorCamera.GetCameraProjectionMatrix(windowWidth/windowHeight);
        Matrix cameraView = m_EditorCamera.GetCameraViewMatrix();
            
        // recompose matrix
        float transformMatrix[4 * 4];
        ImGuizmo::RecomposeMatrixFromComponents(Vector3ToFloat(tc->GetTransform().Translation), Vector3ToFloat(tc->GetTransform().Rotation), Vector3ToFloat(tc->GetTransform().Scale), transformMatrix);
            
        // snapping
        bool snap = IsKeyDown(KEY_LEFT_CONTROL);
        float snapValue = 0.5f;
        if (m_CurrentGizmo == ImGuizmo::OPERATION::ROTATE) {
            snapValue = 45.0f;
        }
        float snapValues[3] = { snapValue, snapValue, snapValue };
            
        ImGuizmo::Manipulate(MatrixToFloat(cameraView), MatrixToFloat(cameraProjection), (ImGuizmo::OPERATION)m_CurrentGizmo, ImGuizmo::LOCAL, transformMatrix, nullptr, snap ? snapValues : nullptr);
            
        if (ImGuizmo::IsUsing())
        {
            float translation[3];
            float rotation[3];
            float scale[3];

            ImGuizmo::DecomposeMatrixToComponents(transformMatrix, translation, rotation, scale);
                
            tc->GetTransform().Translation = {translation[0], translation[1], translation[2]};
            tc->GetTransform().Rotation = {rotation[0], rotation[1], rotation[2]};
            tc->GetTransform().Scale = {scale[0], scale[1], scale[2]};
        }
    }
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

void EditorLayer::OpenFile()
{
    // @TODO: Open project
    
    const char* lFilterPatterns[] = {"*.spectral"};
    const char* filePath = tinyfd_openFileDialog("Open file...", ""/* set file path from filesystem !! */, 1, lFilterPatterns, nullptr, 0);
    
    if (!filePath) {
        // print error
        return;
    }
    LoadFile(filePath);
    
    SP_CLIENT_LOG_INFO("Opened file: {0}", filePath); // we're gonna need a relative path
}

void EditorLayer::SaveFile()
{
    const char* lFilterPatterns[] = {"*.spectral"};
    
    const char* filePath = tinyfd_saveFileDialog("Save as...", "", 1, lFilterPatterns, nullptr);
    
    if (!filePath) {
        // print error
        return;
    }
    
    Spectral::SceneSerializer serializer(m_ActiveScene);
    serializer.Serialize(filePath);
    
    SP_CLIENT_LOG_INFO("Saved file: {0}", filePath);
}

void EditorLayer::LoadFile(const std::string& path)
{
    std::filesystem::path filePath(path);
    m_ActiveScene = std::make_shared<Spectral::Scene>(filePath.stem().string());
    m_HierarchyPanel.SetContext(m_ActiveScene); // update context
    
    Spectral::SceneSerializer serializer(m_ActiveScene);
    if (serializer.Deserialize(path)) {
        m_CurrentScenePath = path;
    }
}

void EditorLayer::QuickSave()
{
    if (!m_CurrentScenePath.empty())
    {
        Spectral::SceneSerializer serializer(m_ActiveScene);
        serializer.Serialize(m_CurrentScenePath);
        
        SP_CLIENT_LOG_INFO("Quick Save: {0}", m_CurrentScenePath);
    }
    else
    {
        SaveFile();
    }
}
