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

#include <filesystem>

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
            std::vector<uint64_t> keys;
            for (const auto& [k, _] : m_Context->m_ObjectRegistry)
            {
                keys.push_back(k);
            }
            
            for (const auto& key : keys)
            {
                const auto& object = m_Context->m_ObjectRegistry[key];
                DrawObjectNode(object.get());
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
                    m_Context->CreateObject<Object>(UUID());
                }
                if (ImGui::MenuItem("Create Static Object"))
                {
                    m_Context->CreateObject<StaticObject>(UUID());
                }
                if (ImGui::MenuItem("Create Runtime Object"))
                {
                    m_Context->CreateObject<RuntimeObject>(UUID());
                }
                // @TODO: Add line spaces
                
                ImGui::EndPopup();
            }
        }
        ImGui::End();
        
        
        ImGui::Begin("Properties");
        if (m_SelectedContext != nullptr) {
            DrawProperties();
        }
        ImGui::End();
    }

    void HierarchyPanel::SetContext(const std::shared_ptr<Scene>& context) 
    {
        m_Context = context;
        m_SelectedContext = nullptr;
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
        
        bool removeNode = false;
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Delete Entity")) {
                removeNode = true;
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
        
        if (removeNode)
        {
            if (m_SelectedContext == object) {
                m_SelectedContext = nullptr;
            }
            
            SP_LOG_INFO("Trying to delete, here is uuid {0}", object->GetUUID());
            
            if (!m_Context->RemoveObject(object->GetUUID()))
            {
                // print some error
            }
            
            removeNode = false;
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
            ImGui::Checkbox("IsActive?", &component.Active);
            ImGui::Checkbox("Debug?", &component.Debug);
            
            auto camera = component.Camera;
            
            bool isPerspective = (camera->GetCamera3D().projection == CAMERA_PERSPECTIVE) ? true : false;
            
            ImGui::Text("Projection");
            if (ImGui::BeginCombo("##Projection", isPerspective ? "Perspective" : "Orthographic"))
            {
                if (ImGui::Selectable("Perspective", isPerspective)) 
                {
                    isPerspective = true;
                    camera->ChangeProjection(CAMERA_PERSPECTIVE);
                }
                
                if (ImGui::Selectable("Orthographic", !isPerspective)) 
                {
                    isPerspective = false;
                    camera->ChangeProjection(CAMERA_ORTHOGRAPHIC);
                }
                
                ImGui::EndCombo();
            }
            
            ImGui::Text("FOV");
            ImGui::SliderFloat("##FOVSlider", &camera->GetCamera3D().fovy, 1.0f, 180.0f, "%.1f");
        });
        
        
        DrawComponent<TransformComponent>("Transform", /*calling anonymous function*/ [](auto& component) {
            DrawVector3Control("Translation", component.Transform.Translation);
            DrawVector3Control("Rotation", component.Transform.Rotation);
            DrawVector3Control("Scale", component.Transform.Scale);
        });
        
        
        DrawComponent<SpriteComponent>("Sprite", /*calling anonymous function*/ [](auto& component) {            
            ImGui::Button("Load Texture", ImVec2(100.0f, 0.0f));
            
            if (ImGui::BeginDragDropTarget()) {
                
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE_PAYLOAD"))
                {
                    const char* path = (const char*)payload->Data;
                    std::filesystem::path texturePath(path); // @TODO: We actually don't need this step - there is a implicit from const char* to std::string
                    component.SpriteTexture = *TextureManager::LoadTexture(texturePath.string()).get();
                }
                
                ImGui::EndDragDropTarget();
            }
            
            ImGui::ColorEdit4("Tint Color", (float*)(void*)&component.Tint);
        });
        
        
        DrawComponent<AnimationComponent>("Animation", /*calling anonymous function*/ [](auto& component) {
            
        });
        
        
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
