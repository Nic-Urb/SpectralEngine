//
//  HierarchyPanel.cpp
//  SpectralEditor
//
//  Created by Nicolas U on 14.04.24.
//

// @TODO: Disable warnings #ifndef ...

#include "HierarchyPanel.hpp"

#include "imgui.h"
#include "imgui_internal.h"

namespace Spectral {

    HierarchyPanel::HierarchyPanel(const std::shared_ptr<Scene>& context)
    {
        SetContext(context);
    }

    void HierarchyPanel::OnImGuiRender()
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
            DisplayAddComponentEntry<CameraComponent>("Camera");
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
        
        
        DrawComponent<CameraComponent>("Camera", /*calling anonymous function*/ [](auto& component) {
            ImGui::Checkbox("IsActive?", &component.IsActive());
            
            // @TODO: Position control sliders
            /*auto camera = component.GetCamera();
            ImGui::Text("Camera Position");
            ImGui::DragFloat("x", &camera->GetPosition().x, 0.01f, 0.0f, 0.0f, "%.2f");
            ImGui::DragFloat("y", &camera->GetPosition().y, 0.01f, 0.0f, 0.0f, "%.2f");*/
        });
        
        
        DrawComponent<TransformComponent>("Transform", /*calling anonymous function*/ [](auto& component) {
            DrawVector3Control("Translation", component.GetTransform().Translation);
            DrawVector3Control("Rotation", component.GetTransform().Rotation);
            DrawVector3Control("Scale", component.GetTransform().Scale);
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
                component.SetSprite(TextureManager::GetTexture("assets/textures/Checkerboard.png").get());
            }
            
            ImGui::ColorEdit4("Tint Color", component.GetTint());
        });
        
        
        DrawComponent<AnimationComponent>("Animation", /*calling anonymous function*/ [](auto& component) {
            
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

    void HierarchyPanel::DrawVector3Control(const std::string& name, Vector3& values) 
    {
        const float sliderSpeed = 0.05f;
        
        ImGui::PushID(name.c_str());
        
        ImGui::Text(name.c_str());
        
        ImGui::PushMultiItemsWidths(3, 120/*width size*/);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 4.0f, 4.0f });
        
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            ImGui::Button("X");
            ImGui::SameLine();
            ImGui::DragFloat("##X", &values.x, sliderSpeed, 0.0f, 0.0f, "%.2f");
        ImGui::PopStyleColor(2);
        
        ImGui::SameLine();
        
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.8f, 0.15f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{ 0.1f, 0.8f, 0.1f, 1.0f });
            ImGui::Button("Y");
            ImGui::SameLine();
            ImGui::DragFloat("##Y", &values.y, sliderSpeed, 0.0f, 0.0f, "%.2f");
        ImGui::PopStyleColor(2);
        
        ImGui::SameLine();
        
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.15f, 0.8f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{ 0.1f, 0.15f, 0.8f, 1.0f });
            ImGui::Button("Z");
            ImGui::SameLine();
            ImGui::DragFloat("##Z", &values.z, sliderSpeed, 0.0f, 0.0f, "%.2f");
        ImGui::PopStyleColor(2);
        
        
        ImGui::PopItemWidth();
        ImGui::PopStyleVar();
        
        
        ImGui::PopID();
    }

}
