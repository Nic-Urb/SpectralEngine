//
//  EditorLayer.hpp
//  SpectralEditor
//
//  Created by Nicolas U on 04.04.24.
//
#include "Spectral.h"
#include "Renderer/EditorCamera.hpp"
#include "Panels/HierarchyPanel.hpp"

class EditorLayer : public Spectral::Layer // @TODO: If selected context contains camera component -> draw visualization to show a camera position
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
    std::shared_ptr<Spectral::Scene> m_ActiveScene;
    Spectral::EditorCamera m_EditorCamera;
    Spectral::HierarchyPanel m_HierarchyPanel;
    
    RenderTexture m_Framebuffer;
    
    enum class SceneState {
        Edit = 0, Play = 1
    };
    SceneState m_CurrentState = SceneState::Edit;
    
private:
    void DrawToolbar();
    
    void OnRuntimeStart();
    void OnRuntimeStop();
};
