//
//  EditorLayer.cpp
//  SpectralEditor
//
//  Created by Nicolas U on 04.04.24.
//

#include "EditorLayer.hpp"

#include "materialdesign-main/IconsMaterialDesign.h"

#include "Core/SceneSerializer.hpp"
#include "Panels/StatisticsPanel.hpp"
#include "Renderer/Shaders.hpp"

#include "rlImGui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "raymath.h"
#include "ImGuizmo.h"
#include "tinyfiledialogs.h"

#include <filesystem>

namespace Spectral {

    void EditorLayer::OnAttach()
    {
        SP_CLIENT_LOG_INFO("EDITOR-APP::OnAttach");
        
        m_ActiveScene = std::make_shared<Scene>("TestScene");
        
        m_EditorCamera = EditorCamera();
        m_HierarchyPanel= HierarchyPanel(m_ActiveScene);
        m_ContentBrowserPanel = std::make_unique<ContentBrowserPanel>();
        m_StatsPanel = StatisticsPanel(m_ActiveScene);
        
        m_Framebuffer = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
        SetTextureFilter(m_Framebuffer.texture, TEXTURE_FILTER_BILINEAR); // @TODO: Move
    }

    void EditorLayer::OnDetach()
    {
        SP_CLIENT_LOG_INFO("EDITOR-APP::OnDetach");
        UnloadRenderTexture(m_Framebuffer);
    }

    void EditorLayer::OnUpdate(Timestep ts)
    {
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
        
        if (IsWindowResized())
        {
            UnloadRenderTexture(m_Framebuffer);
            m_Framebuffer = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
        }
        
        if (m_CurrentState == SceneState::Play)
        {
            m_ActiveScene->OnUpdateRuntime(ts);
        } else {
            m_ActiveScene->OnUpdateEditor(ts);
            MousePicking();
        }
        
        if (m_ViewportFocused)
        {
            if (m_CurrentState == SceneState::Edit)
            {
                m_EditorCamera.OnUpdate(ts);
            }
            
            // handle input events for gizmo shortcuts
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
        }
    }

    void EditorLayer::OnRender()
    {
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
        
        DrawTitlebar();
        
        // render panels
        m_HierarchyPanel.OnImGuiRender();
        m_ContentBrowserPanel->OnImGuiRender();
        
        if (m_StatsPanel.IsActive()) {
            m_StatsPanel.OnImGuiRender();
        }

        // render viewport
        char buffer[128];
        std::string name = (m_CurrentState == SceneState::Edit) ? "Edit Mode" : "Play Mode";
        snprintf(buffer, sizeof(buffer), "%s (%s)###Viewport",m_ActiveScene->GetName().c_str() ,name.c_str());
        
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin(buffer, nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);
            m_ViewportFocused = ImGui::IsWindowHovered();
        
            ImVec2 viewportRegion = ImGui::GetContentRegionAvail();
       
            // store viewport info
            m_ViewportRect = { ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMin().x, ImGui::GetWindowPos().y + ImGui::GetWindowContentRegionMin().y, viewportRegion.x, viewportRegion.y };
        
            // @NOTE: temporary workaround for shaders
            BeginShaderMode(Shaders::GetLightingShader());
                // render the framebuffer into a rectangle area and ensure it fits within the ImGui window
                DrawTexturePro(m_Framebuffer.texture, (Rectangle){0, 0, (float)m_Framebuffer.texture.width, -(float)m_Framebuffer.texture.height}, m_ViewportRect, (Vector2){0, 0}, 0.0f, WHITE);
            EndShaderMode();
        
            //ImGui::Image((ImTextureID)&m_Framebuffer.texture, viewportRegion, ImVec2(0,1), ImVec2(1,0)); // render framebuffer to imgui viewport
        
            ImRect dropTargetRect(ImVec2(m_ViewportRect.x, m_ViewportRect.y), ImVec2(m_ViewportRect.x + m_ViewportRect.width, m_ViewportRect.y + m_ViewportRect.height));
        
            if (ImGui::BeginDragDropTargetCustom(dropTargetRect, ImGui::GetID("ViewportDropTarget")))
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
        ImGui::PopStyleVar();
    }

    void EditorLayer::DrawTitlebar()
    {
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
        
        // Create main menu titlebars
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(100.0f, 0.0f));
        
        if (ImGui::BeginViewportSideBar("##MainMenuBar", ImGui::GetMainViewport(), ImGuiDir_Up, 23.0f, flags))
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0f, 5.0f));
            if (ImGui::BeginMenuBar())
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
                    bool isActive = m_StatsPanel.IsActive();
                    if (ImGui::MenuItem("Engine Stats", nullptr, &isActive)) {
                        m_StatsPanel.SetActive(isActive);
                    }
                    
                    ImGui::EndMenu();
                }
                
                if (ImGui::BeginMenu("Help"))
                {
                    
                    ImGui::EndMenu();
                }
                
                ImGui::EndMenuBar();
            }
            ImGui::End();
            ImGui::PopStyleVar();
        }
        
        ImGui::PopStyleVar();
        
        // Create secondary menu bar
        if (ImGui::BeginViewportSideBar("##SecondaryMenuBar", ImGui::GetMainViewport(), ImGuiDir_Up, ImGui::GetFrameHeight(), flags))
        {
            if (ImGui::BeginMenuBar()) {
                DrawToolbar();
                ImGui::EndMenuBar();
            }
            ImGui::End();
        }
    }

    void EditorLayer::DrawToolbar()
    {
        ImVec2 buttonSize = ImVec2(35.0f, ImGui::GetFrameHeight());
        
        
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.08f, 0.08f, 0.08f, 1.0f));
        
        if (ImGui::Button(ICON_MD_SAVE_ALT, buttonSize)) {
            QuickSave();
        }

        ImGui::SetCursorPosX(ImGui::GetContentRegionMax().x - buttonSize.x);
        if (ImGui::Button(ICON_MD_SETTINGS, buttonSize)) {
            //  @TODO: Show settings
        }
        
        ImGui::PopStyleColor();
        
        ImVec4 greenColor = ImVec4(0.0f, 0.78f, 0.0f, 1.0f);
        ImVec4 grayColor = ImVec4(0.78f, 0.78f, 0.78f, 1.0f);
        
        static ImVec4 playColor = greenColor;
        static ImVec4 stopColor =  grayColor;
        
        float maxRegion = ImGui::GetContentRegionMax().x;
        float startButtonPos = maxRegion * 0.5f -/* item count */2 * 0.5f * buttonSize.x;
        ImGui::SetCursorPosX(startButtonPos);
        
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.22f, 0.22f, 0.22f, 1.0f));
        
        ImGui::PushStyleColor(ImGuiCol_Text, playColor);
        if (ImGui::Button(ICON_MD_PLAY_ARROW, buttonSize)) {
            OnRuntimeStart();
            
            playColor = grayColor;
            stopColor = greenColor;
        }
        ImGui::PopStyleColor();
    
        // @TODO: Add pause
        
        ImGui::PushStyleColor(ImGuiCol_Text, stopColor);
        if (ImGui::Button(ICON_MD_STOP, buttonSize)) {
            OnRuntimeStop();
            
            playColor = greenColor;
            stopColor = grayColor;
        }
            
        ImGui::PopStyleColor(2);
    }

    void EditorLayer::OnGizmoUpdate()
    {
        Entity selectedEntity = m_HierarchyPanel.GetSelectedEntity();
        if (selectedEntity && m_CurrentGizmo != -1)
        {
            auto& tc = selectedEntity.GetComponent<TransformComponent>();
            
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
            
            float windowWidth = (float)ImGui::GetWindowWidth();
            float windowHeight = (float)ImGui::GetWindowHeight();
            
            ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
            
            Matrix cameraProjection = m_EditorCamera.GetCameraProjectionMatrix(windowWidth/windowHeight);
            Matrix cameraView = m_EditorCamera.GetCameraViewMatrix();
            
            // recompose matrix
            float transformMatrix[4 * 4];
            ImGuizmo::RecomposeMatrixFromComponents(Vector3ToFloat(tc.Translation), Vector3ToFloat(tc.Rotation), Vector3ToFloat(tc.Scale), transformMatrix);
        
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
                
                tc.Translation = {translation[0], translation[1], translation[2]};
                tc.Rotation = {rotation[0], rotation[1], rotation[2]};
                tc.Scale = {scale[0], scale[1], scale[2]};
            }
        }
    }
    
    void EditorLayer::MousePicking()
{
        // @TODO: Fix mouse position
        // @TODO: Fix sprite bounds colllision
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && m_ViewportFocused && !ImGuizmo::IsOver() && !ImGuizmo::IsUsing())
        {
            Vector2 relativeMousePosition = (Vector2){(float)GetMousePosition().x - (float)m_ViewportRect.x, (float)GetMousePosition().y - (float)m_ViewportRect.y};
            
            Ray mouseRay = GetScreenToWorldRayEx(relativeMousePosition, m_EditorCamera.GetCamera3D(), m_ViewportRect.width, m_ViewportRect.height);

            RayCollision collision = { 0 };
            collision.hit = false;
            collision.distance = FLT_MAX;
            
            // handle sprite
            {
                auto view = m_ActiveScene->m_Registry.view<SpriteComponent>();
                for (auto entity : view)
                {
                    Entity entt = { entity, m_ActiveScene.get() };
           
                    auto& sprite = entt.GetComponent<SpriteComponent>();
                        
                    //RayCollision spriteCollision = GetRayCollisionQuad(mouseRay, sprite.Bounds[0], sprite.Bounds[1], sprite.Bounds[2], sprite.Bounds[3]);
                        
                    /*if (spriteCollision.hit && spriteCollision.distance < collision.distance) {
                        collision = spriteCollision;
                        m_SelectedEntity = entt;
                    }*/
                }
            }
            
            
            // handle model
            /*{
                auto view = m_ActiveScene->m_Registry.view<ModelComponent>();
                for (auto entity : view)
                {
                    Entity entt = { entity, m_ActiveScene.get() };
           
                    auto& model = entt.GetComponent<ModelComponent>();
                        
                    RayCollision modelCollision; //= GetRayCollisionMesh(mouseRay, Mesh mesh, Matrix transform);
                        
                    if (modelCollision.hit && modelCollision.distance < collision.distance) {
                        collision = modelCollision;
                        m_SelectedEntity = entt;
                    }
                }
            }*/
            
            
            // Update the collision info with the closest hit
            if (collision.hit) {
                m_CollisionInfo = collision;
            } else {
                m_CollisionInfo = { 0 };
                m_SelectedEntity = {};  // Ensure if no hit
            }
            
            if (m_SelectedEntity){
                m_HierarchyPanel.SetSelectedEntity(m_SelectedEntity);
            } else {
                m_HierarchyPanel.SetSelectedEntity({});
            }
            
            // Send collision info to stats panel
            m_StatsPanel.SetCollisionInfo(m_CollisionInfo, m_SelectedEntity ? m_SelectedEntity.GetComponent<IDComponent>().Name : "");
        }
    }
    
    void EditorLayer::OnRuntimeStart()
    {
        if (m_CurrentState == SceneState::Play) {
            return;
        }
        
        m_CurrentState = SceneState::Play;
        
        // m_ActiveScene = new Scene();
        m_ActiveScene->OnRuntimeStart();
    }
    
    void EditorLayer::OnRuntimeStop()
    {
        if (m_CurrentState == SceneState::Edit) {
            return;
        }
        
        m_CurrentState = SceneState::Edit;
        
        // m_ActiveScene = EditorScene;
        m_ActiveScene->OnRuntimeEnd();
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
        const char* filePath = tinyfd_saveFileDialog("Save as...", "untitled.spectral", 1, lFilterPatterns, nullptr);
        
        if (!filePath) {
            // print error
            return;
        }
        
        SceneSerializer serializer(m_ActiveScene);
        serializer.Serialize(filePath);
        
        SP_CLIENT_LOG_INFO("Saved file: {0}", filePath);
    }
    
    void EditorLayer::LoadFile(const std::string& path)
    {
        std::filesystem::path filePath(path);
        m_ActiveScene = std::make_shared<Scene>(filePath.stem().string());
        
        // update context of the panels
        m_HierarchyPanel.SetContext(m_ActiveScene);
        m_StatsPanel.SetContext(m_ActiveScene);
        
        SceneSerializer serializer(m_ActiveScene);
        if (serializer.Deserialize(path)) {
            m_CurrentScenePath = path;
        }
    }
    
    void EditorLayer::QuickSave()
    {
        if (!m_CurrentScenePath.empty())
        {
            SceneSerializer serializer(m_ActiveScene);
            serializer.Serialize(m_CurrentScenePath);
            
            SP_CLIENT_LOG_INFO("Quick Save: {0}", m_CurrentScenePath);
        }
        else
        {
            SaveFile();
        }
    }
    
}
