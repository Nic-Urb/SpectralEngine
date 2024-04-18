//
//  EditorLayer.hpp
//  SpectralEditor
//
//  Created by Nicolas U on 04.04.24.
//
#include "Spectral.h"
#include "Renderer/EditorCamera.hpp"
#include "Panels/HierarchyPanel.hpp"

class EditorLayer : public Spectral::Layer
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
    std::shared_ptr<Spectral::Scene> m_EditorScene;
    Spectral::EditorCamera m_EditorCamera;
    Spectral::HierarchyPanel m_HierarchyPanel;
    
    RenderTexture m_Framebuffer;
};
