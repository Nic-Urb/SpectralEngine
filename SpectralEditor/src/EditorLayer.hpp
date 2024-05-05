//
//  EditorLayer.hpp
//  SpectralEditor
//
//  Created by Nicolas U on 04.04.24.
//
#include "Spectral.h"
#include "Renderer/EditorCamera.hpp"
#include "Panels/HierarchyPanel.hpp"
#include "Panels/ContentBrowserPanel.hpp"

#include <filesystem>

class EditorLayer : public Spectral::Layer // @TODO: Use Spectral namespace
{
public:
    EditorLayer();
    ~EditorLayer() override
    {
        UnloadRenderTexture(m_Framebuffer);
    }
    
    inline void OnAttach() override
    {
        SP_CLIENT_LOG_INFO("EDITOR-APP::OnAttach");
        // @TODO: On attach is never reached !!
        // @TODO: Put init stuff in here ??
    }
    
    inline void OnDetach() override
    {
        SP_CLIENT_LOG_INFO("EDITOR-APP::OnDetach"); // @TODO: Check if this function is even called
        //UnloadRenderTexture(m_Framebuffer);
    }
    
    void OnUpdate(Spectral::Timestep ts) override;
    void OnImGuiRender() override;
    
private:
    enum class SceneState {
        Edit = 0,
        Play = 1
    };
    
private:
    std::shared_ptr<Spectral::Scene> m_ActiveScene;
    Spectral::EditorCamera m_EditorCamera;
    Spectral::HierarchyPanel m_HierarchyPanel;
    ContentBrowserPanel m_ContentBrowserPanel;
    
    RenderTexture m_Framebuffer;
    
    SceneState m_CurrentState = SceneState::Edit;
    int m_CurrentGizmo = -1;
    
    std::string m_CurrentScenePath; // path that points to current scene
    
private:
    void DrawToolbar();
    
    void OnGizmoRender();
    void OnGizmoUpdate();
    
    void OnRuntimeStart();
    void OnRuntimeStop();
    
    void OpenFile();
    void SaveFile();
    void LoadFile(const std::string& path);
    void QuickSave();
};
