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
        void OnAttach() override;
        void OnDetach() override;
    
        void OnUpdate(Timestep ts) override;
        void OnRender() override;
        void OnImGuiRender() override;
    
    private:
        enum class SceneState { Edit = 0, Play = 1 };
    
    private:
        std::shared_ptr<Scene> m_ActiveScene;
        EditorCamera m_EditorCamera;
    
        HierarchyPanel m_HierarchyPanel;
        StatisticsPanel m_StatsPanel;
        std::unique_ptr<ContentBrowserPanel> m_ContentBrowserPanel;
    
        RenderTexture m_Framebuffer;
    
        bool m_ViewportFocused;
        Rectangle m_ViewportRect;
    
        SceneState m_CurrentState = SceneState::Edit;
        int m_CurrentGizmo = -1;
    
        Entity m_SelectedEntity; // entity selected by mouse picking
        RayCollision m_CollisionInfo;
    
        std::string m_CurrentScenePath; // path that points to current scene
    
    private:
        void DrawTitlebar();
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


