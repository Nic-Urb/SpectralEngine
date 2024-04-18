//
//  HierarchyPanel.cpp
//  SpectralEditor
//
//  Created by Nicolas U on 14.04.24.
//

#include "HierarchyPanel.hpp"

#include "imgui.h"

namespace Spectral {

    HierarchyPanel::HierarchyPanel(const std::shared_ptr<Scene>& context)
    {
        SetContext(context);
    }

    void HierarchyPanel::OnImGuiRender() // @TODO: RENAME WORLD TO SCENE !!
    {
        ImGui::Begin("Scene");
        
        if (m_Context)
        {
            // temp approach used to avoid modifying the container while iterating over it
            std::vector<UUID> keys;
            for (const auto& [k, _] : m_Context->m_ObjectRegistry)
            {
                keys.push_back(k);
            }
            
            for (const auto& key : keys)
            {
                const auto& object = m_Context->m_ObjectRegistry[key];
                DrawObjectNode(object.get());
                
                if (m_RemoveNode)
                    RemoveObjectNode(key);
            }
            
            if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
            {
                m_SelectedContext = nullptr;
            }
            
            // Right-click on blank space
            if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
            {
                if (ImGui::MenuItem("Create Empty Object"))
                {
                    // create object from context
                    m_Context->CreateObject<Object>(/* name */);
                }
                if (ImGui::MenuItem("Create Static Object"))
                {
                    m_Context->CreateObject<StaticObject>();
                }
                if (ImGui::MenuItem("Create Runtime Object"))
                {
                    m_Context->CreateObject<RuntimeObject>(/*params*/);
                }
                // @TODO: Add line space
                // add here all new created objects
                if (ImGui::MenuItem("Create Testing Object"))
                {
                    m_Context->CreateObject<RuntimeObject>(/*params*/);
                }
                
                ImGui::EndPopup();
            }
            
        }
        ImGui::End();
        
        
        ImGui::Begin("Properties");
        if (m_SelectedContext) {
            DrawProperties();
        }
        ImGui::End();
    }

    void HierarchyPanel::SetContext(const std::shared_ptr<Scene>& context) 
    {
        m_Context = context;
    }

    void HierarchyPanel::DrawObjectNode(Object* object)
    {
        ImGuiTreeNodeFlags flags = ((m_SelectedContext == object) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
        flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
        bool opened = ImGui::TreeNodeEx((void*)object, flags, object->GetName().c_str());
        
        if (ImGui::IsItemClicked())
        {
            m_SelectedContext = object;
        }
        
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Delete Entity"))
            {
                m_RemoveNode = true;
            }
            ImGui::EndPopup();
        }
        
        if (opened)
        {
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
            bool opened = ImGui::TreeNodeEx((void*)9817239, flags, object->GetName().c_str()); // (void*)9817239 some random id just for testing
            if (opened)
            {
                ImGui::TreePop();
            }
                        
            ImGui::TreePop();
        }
        
    }

    void HierarchyPanel::DrawProperties() 
    {
        if (ImGui::Button("Add Component"))
        {
            ImGui::OpenPopup("AddComponent");
        }

        if (ImGui::BeginPopup("AddComponent"))
        {
            DisplayAddComponentEntry<TransformComponent>("Transform");
            DisplayAddComponentEntry<SpriteComponent>("Sprite");
            DisplayAddComponentEntry<AnimationComponent>("Animation");
            
            ImGui::EndPopup();
        }
        
        const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
        bool open = ImGui::TreeNodeEx((void*)9182213, treeNodeFlags, "General");
        if (open)
        {
            const std::string& name = m_SelectedContext->GetName();
            char buffer[128];
            strncpy(buffer, name.c_str(), sizeof(buffer));
            ImGui::Text("Name");
            ImGui::SameLine();
            if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
            {
                m_SelectedContext->SetName(buffer);
            }
            ImGui::TreePop();
        }
        
        DrawComponent<TransformComponent>("Transform", /*calling anonymous function*/ [](auto& component) {
            ImGui::Text("Relative translation");
            DrawTransformControl(component);
            
        });
        
        
        DrawComponent<SpriteComponent>("Sprite", /*calling anonymous function*/ [](auto& component) {
            static char buffer[128];
            ImGui::InputText("##Texture", buffer, sizeof(buffer));
            
            if (ImGui::Button("Load Texture", ImVec2(100.0f, 0.0f)))
            {
                //TextureManager::LoadTexture(std::string(buffer));
                //component.SetSprite(TextureManager::GetTexture(std::string(buffer)));
                // @TODO: Replace with upper code !!
                TextureManager::LoadTexture("assets/textures/Checkerboard.png");
                component.SetSprite(TextureManager::GetTexture("assets/textures/Checkerboard.png"));
            }
            
            ImGui::ColorEdit4("Tint Color", component.GetTint());
        });
        
        DrawComponent<AnimationComponent>("Animation", /*calling anonymous function*/ [](auto& component) {
            
        });
        
        
        DrawComponent<CameraComponent>("Camera", /*calling anonymous function*/ [](auto& component) {
            
            //auto& camera = component.GetCamera();
        });
        
        
    }

    void HierarchyPanel::RemoveObjectNode(UUID uuid)
    {
        if (m_SelectedContext->GetUUID() == uuid)
        {
            m_SelectedContext = nullptr;
        }
        m_Context->RemoveObject(uuid);
    
        m_RemoveNode = false;
    }

    template <typename T, typename F>
    void HierarchyPanel::DrawComponent(const std::string& name, F&& lambda) 
    {
        const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
        
        if (m_SelectedContext)
        {
            if (m_SelectedContext->HasComponent(T::GetComponentId()))
            {
                auto& component = *m_SelectedContext->GetComponent<T>();
                
                bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
                
                bool removeComponent = false;
                if (ImGui::BeginPopup("ComponentSettings"))
                {
                    if (ImGui::MenuItem("Remove component"))
                    {
                        removeComponent = true;
                    }
                    ImGui::EndPopup();
                }
                
                
                if (open)
                {
                    if (ImGui::Button("+", ImVec2{ 20.0f, 20.0f }))
                    {
                        ImGui::OpenPopup("ComponentSettings");
                    }
                    
                    lambda(component);
                    ImGui::TreePop();
                }
                
                
                if (removeComponent)
                {
                    m_SelectedContext->RemoveComponent(T::GetComponentId());
                }
                                
            }
        }
    }

    template <typename T>
    void HierarchyPanel::DisplayAddComponentEntry(const std::string& entryName) 
    {
        if (m_SelectedContext)
        {
            if (!m_SelectedContext->HasComponent(T::GetComponentId()))
            {
                if (ImGui::MenuItem(entryName.c_str()))
                {
                    m_SelectedContext->AddComponent<T>();
                    ImGui::CloseCurrentPopup();
                }
            }
        }
    }

    void HierarchyPanel::DrawTransformControl(auto& component) 
    {
        // ---- Translate
        ImGui::Text("Translate Control");
        ImGui::DragFloat("##X", &component.GetTranslation().x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::DragFloat("##Y", &component.GetTranslation().y, 0.1f, 0.0f, 0.0f, "%.2f");
        
        // ---- Rotate
        ImGui::Text("Rotation Control");
        ImGui::DragFloat("##Rotation", &component.GetRotation(), 0.1f, 0.0f, 0.0f, "%.2f");
        
        // ---- Scale
        ImGui::Text("Scale Control");
        ImGui::DragFloat("##ScaleX", &component.GetScale().x, 0.01f, 0.0f, 0.0f, "%.2f");
        ImGui::DragFloat("##ScaleY", &component.GetScale().y, 0.01f, 0.0f, 0.0f, "%.2f");
    }

}
