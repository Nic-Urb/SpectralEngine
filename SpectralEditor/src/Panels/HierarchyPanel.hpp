//
//  HierarchyPanel.hpp
//  SpectralEditor
//
//  Created by Nicolas U on 14.04.24.
//
#include "Spectral.h"

namespace Spectral {
    
    class HierarchyPanel
    {
    public:
        HierarchyPanel() = default;
        HierarchyPanel(const std::shared_ptr<Scene>& context);
        
        void OnImGuiRender();

        Entity GetSelectedEntity() const { return m_SelectedEntity; }
        void SetSelectedEntity(Entity ent) { m_SelectedEntity = ent; }
        
        void SetContext(const std::shared_ptr<Scene>& context);
        
    private:
        std::shared_ptr<Scene> m_Context;
        Entity m_SelectedEntity;
        
    private:
        void DrawEntityNode(Entity entity);
        void DrawProperties();
        
        template <typename T, typename F>
        void DrawComponent(const std::string& name, F&& lambda, const bool removable = true);
        
        template <typename T>
        void DisplayAddComponentEntry(const std::string& entryName);
        
        static void DrawVector3Control(const std::string& name, Vector3& values);
        
    };
}

// @TODO: Implement groups
// unused
struct NodeGroup
{
    std::vector<Spectral::Entity> m_GroupEntity;
};
