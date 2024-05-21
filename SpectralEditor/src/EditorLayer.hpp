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
#include "Panels/StatisticsPanel.hpp"

namespace Spectral {
    class EditorLayer : public Layer
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
    
        void OnUpdate(Timestep ts) override;
        void OnImGuiRender() override;
    
    private:
        enum class SceneState {
            Edit = 0,
            Play = 1
        };
    
    private:
        std::shared_ptr<Scene> m_ActiveScene;
        EditorCamera m_EditorCamera;
    
        // Panels where lifetime is tied to the EditorLayer class
        HierarchyPanel m_HierarchyPanel;
        ContentBrowserPanel m_ContentBrowserPanel;
        StatisticsPanel m_StatsPanel;
    
        RenderTexture m_Framebuffer;
    
        bool m_ViewportFocused;
        Rectangle m_ViewportRect;
    
        SceneState m_CurrentState = SceneState::Edit;
        int m_CurrentGizmo = -1;
    
        Object* m_SelectedObject = nullptr; // object selected by mouse picking
        RayCollision m_CollisionInfo;
    
        std::string m_CurrentScenePath; // path that points to current scene
    
    private:
        void DrawToolbar();
    
        void OnGizmoRender();
        void OnGizmoUpdate();
    
        void MousePicking();
    
        void OnRuntimeStart();
        void OnRuntimeStop();
    
        void OpenFile();
        void SaveFile();
        void LoadFile(const std::string& path);
        void QuickSave();
    };
}


