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
        
        Object* GetSelectedContext() { return m_SelectedContext; }
        void SetContext(const std::shared_ptr<Scene>& context);
        
    private:
        std::shared_ptr<Scene> m_Context;
        Object* m_SelectedContext = nullptr;
        bool m_RemoveNode = false;
        
    private:
        void DrawObjectNode(Object* object);
        void DrawProperties();
        
        void RemoveObjectNode(UUID uuid);
        
        template <typename T, typename F>
        void DrawComponent(const std::string& name, F&& lambda);
        
        template <typename T>
        void DisplayAddComponentEntry(const std::string& entryName);
        
        static void DrawVector3Control(const std::string& name, Vector3& values);
        
    };
}

// @TODO: Implement groups
// unused
struct NodeGroup
{
    std::vector<Spectral::Object*> m_GroupObjects;
};
