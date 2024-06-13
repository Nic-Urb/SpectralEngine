//
//  HierarchyPanel.cpp
//  SpectralEditor
//
//  Created by Nicolas U on 14.04.24.
//

// @TODO: Disable warnings #ifndef ...

#include "HierarchyPanel.hpp"

#include "materialdesign-main/IconsMaterialDesign.h"

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
        ImGui::Begin(ICON_MD_LIST "Scene");
        
        if (m_Context)
        {
            for (auto entityID : m_Context->m_Registry.view<entt::entity>())
            {
                Entity entity{ entityID, m_Context.get() };
                DrawEntityNode(entity);
            }
            
            if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
            {
                m_SelectedEntity = {};
            }
            
            // Right-click on blank space
            if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
            {
                if (ImGui::MenuItem("Empty Entity"))
                {
                    m_Context->CreateEntity(UUID(), "Entity");
                }
                
                if (ImGui::MenuItem("Sprite"))
                {
                    Entity entity = m_Context->CreateEntity(UUID(), "Sprite");
                    entity.AddComponent<SpriteComponent>();
                }
                
                if (ImGui::MenuItem("Model"))
                {
                    Entity entity = m_Context->CreateEntity(UUID(), "Model");
                    entity.AddComponent<ModelComponent>();
                }
                
                if (ImGui::MenuItem("Camera"))
                {
                    Entity entity = m_Context->CreateEntity(UUID(), "Camera");
                    entity.AddComponent<CameraComponent>();
                }
                
                if (ImGui::MenuItem("Actor"))
                {
                    Entity entity = m_Context->CreateEntity(UUID(), "Actor");
                    // @TODO: Temp
                    //entity.AddComponent<NativeScriptComponent>().Bind<PlayerController>();
                    // @TODO: Add ScriptComponent
                    // @TODO: Add Physics
                }
                
                if (ImGui::MenuItem("Light"))
                {
                    m_Context->CreateEntity(UUID(), "Light");
                    // @TODO: Add LightComponent
                }
                
                ImGui::EndPopup();
            }
        }
        ImGui::End();
        
        
        ImGui::Begin(ICON_MD_EDIT_NOTE "Properties");
        if (m_SelectedEntity) {
            DrawProperties();
        }
        ImGui::End();
    }

    void HierarchyPanel::SetContext(const std::shared_ptr<Scene>& context) 
    {
        m_Context = context;
        m_SelectedEntity = {};
    }

    void HierarchyPanel::DrawEntityNode(Entity entity)
    {
        ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
        flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
        const char* name = entity.GetComponent<IDComponent>().Name.c_str();
        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, name);
        
        if (ImGui::IsItemClicked())
        {
            m_SelectedEntity = entity;
        }
        
        bool removeNode = false;
        if (ImGui::BeginPopupContextItem(0))
        {
            if (ImGui::MenuItem("Delete ", name)) {
                removeNode = true;
            }
            ImGui::EndPopup();
        }
        
        if (opened)
        {
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
            bool opened = ImGui::TreeNodeEx((void*)9817239, flags, name); // (void*)9817239 some random id just for testing
            if (opened)
            {
                ImGui::TreePop();
            }
                        
            ImGui::TreePop();
        }
        
        if (removeNode)
        {
            m_Context->RemoveEntity(entity);
            
            if (m_SelectedEntity == entity) {
                m_SelectedEntity = {};
            }
            removeNode = false;
        }
        
    }

    void HierarchyPanel::DrawProperties() 
    {
        // setup component general
        {
            auto& idc = m_SelectedEntity.GetComponent<IDComponent>();
            
            ImGui::Text(ICON_MD_VIEW_IN_AR);
            ImGui::SameLine();
            
            const std::string& name = idc.Name;
            char buffer[64];
            strncpy(buffer, name.c_str(), sizeof(buffer));
            ImGui::SameLine();
            ImGui::PushItemWidth(145.0f);
            if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
            {
                idc.Name = buffer;
            }
            ImGui::PopItemWidth();
            
            ImGui::SameLine();
            
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.78f, 0.0f, 1.0f));
            
            if (ImGui::Button("Add Component")) {
                ImGui::OpenPopup("AddComponent");
            }
            
            ImGui::PopStyleColor();
            
            const char* layerMaskStrings[] = { "Static", "Default", "Custom1", "Custom2"};
            const char* currentLayerMaskString = layerMaskStrings[(uint16_t)idc.LayerMask];
            if (ImGui::BeginCombo("LayerMask", currentLayerMaskString))
            {
                for (int i = 0; i < 4; i++)
                {
                    bool isSelected = currentLayerMaskString == layerMaskStrings[i];
                    if (ImGui::Selectable(layerMaskStrings[i], isSelected))
                    {
                        currentLayerMaskString = layerMaskStrings[i];
                        idc.LayerMask = (uint16_t)i;
                    }
                    
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                
                ImGui::EndCombo();
            }
        }
        
        if (ImGui::BeginPopup("AddComponent"))
        {
            DisplayAddComponentEntry<CameraComponent>("Camera");
            DisplayAddComponentEntry<TransformComponent>("Transform");
            DisplayAddComponentEntry<SpriteComponent>("Sprite");
            DisplayAddComponentEntry<ModelComponent>("Model");
            DisplayAddComponentEntry<LuaScriptComponent>("Lua Script");
            DisplayAddComponentEntry<RigidBody2DComponent>("Rigidbody 2D");
            DisplayAddComponentEntry<BoxCollider2DComponent>("BoxCollider 2D");
            DisplayAddComponentEntry<RigidBody3DComponent>("Rigidbody 3D");
            DisplayAddComponentEntry<BoxCollider3DComponent>("BoxCollider 3D");
            
            ImGui::EndPopup();
        }
        
        DrawComponent<CameraComponent>("Camera", /*calling anonymous function*/ [](auto& component) {
            ImGui::Checkbox("IsActive?", &component.Active);
            ImGui::Checkbox("Debug?", &component.Debug);
            
            std::shared_ptr<RuntimeCamera> camera = component.Camera;
            
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
            DrawVector3Control("Translation", component.Translation);
            Vector3 rotation = Vector3Scale(component.Rotation, RAD2DEG); // use rotation in degree instead of radians
            DrawVector3Control("Rotation", rotation);
            component.Rotation = Vector3Scale(rotation, DEG2RAD); // convert back to radians
            DrawVector3Control("Scale", component.Scale);
        }, false);
        
        DrawComponent<SpriteComponent>("Sprite", /*calling anonymous function*/ [](auto& component) {
            ImGui::Button("Load Texture", ImVec2(100.0f, 0.0f));
            
            if (ImGui::BeginDragDropTarget()) {
                
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE_PAYLOAD"))
                {
                    const char* path = (const char*)payload->Data;
                    std::filesystem::path texturePath(path); // @TODO: We actually don't need this step - there is a implicit from const char* to std::string
                    component.SpriteTexture = *AssetsManager::LoadTexture(texturePath.string()).get();
                }
                
                ImGui::EndDragDropTarget();
            }
            
            ImGui::ColorEdit4("Tint Color", (float*)&component.Tint, ImGuiColorEditFlags_NoInputs);
        });
        
        DrawComponent<ModelComponent>("Model", /*calling anonymous function*/ [](auto& component) {
            ImGui::Button("Load Model", ImVec2(100.0f, 0.0f));
            
            if (ImGui::BeginDragDropTarget()) 
            {
                if (const ImGuiPayload* payloadModel = ImGui::AcceptDragDropPayload("3D_PAYLOAD"))
                {
                    const char* path = (const char*)payloadModel->Data;
                    std::filesystem::path modelPath(path); // @TODO: We actually don't need this step - there is a implicit from const char* to std::string
                    component.ModelData = *AssetsManager::LoadModel(modelPath.string()).get();
                }
                
                ImGui::EndDragDropTarget();
            }
            
            ImGui::ColorEdit4("Tint Color", (float*)&component.Tint, ImGuiColorEditFlags_NoInputs);
        });
        
        DrawComponent<LuaScriptComponent>("LuaScript", /*calling anonymous function*/ [](auto& component) {
            
            char buffer[64];
            strncpy(buffer, component.ScriptPath.c_str(), sizeof(buffer));
            
            if (ImGui::InputTextWithHint("Script", "Drag or type file", buffer, sizeof(buffer)))
            {
                component.ScriptPath = buffer;
            }
            
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCRIPT_PAYLOAD"))
                {
                    const char* path = (const char*)payload->Data;
                    std::filesystem::path scriptPath(path);  // @TODO: We actually don't need this step - there is a implicit from const char* to std::string
                    component.ScriptPath = scriptPath;
                }
                
                ImGui::EndDragDropTarget();
            }
        });
        
        
        DrawComponent<RigidBody2DComponent>("Rigidbody 2D", /*calling anonymous function*/ [](auto& component) {
            
            const char* bodyTypeStrings[] = { "Static", "Kinematic", "Dynamic"};
            const char* currentBodyTypeString = bodyTypeStrings[(int)component.Type];
            if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
            {
                for (int i = 0; i < 3; i++)
                {
                    bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
                    if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
                    {
                        currentBodyTypeString = bodyTypeStrings[i];
                        component.Type = (RigidBody2DComponent::BodyType)i;
                    }

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }

                ImGui::EndCombo();
            }

            ImGui::Checkbox("Fixed Rotation", &component.FixedRotation);
            ImGui::Checkbox("Allow Sleep", &component.AllowSleep);
            ImGui::Checkbox("Awake", &component.Awake);
            
            ImGui::DragFloat("Gravity Scale", &component.GravityScale, 0.01f, 0.0f);
        });
        
        DrawComponent<BoxCollider2DComponent>("BoxCollider 2D", /*calling anonymous function*/ [](auto& component) {
            
                //ImGui::DragFloat2("Size", ); // @TODO: Expose size and offset
                ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
        });
        
        DrawComponent<RigidBody3DComponent>("Rigidbody 3D", /*calling anonymous function*/ [](auto& component) {
            
            const char* bodyTypeStrings[] = { "Static", "Kinematic", "Dynamic"};
            const char* currentBodyTypeString = bodyTypeStrings[(int)component.Type];
            if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
            {
                for (int i = 0; i < 3; i++)
                {
                    bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
                    if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
                    {
                        currentBodyTypeString = bodyTypeStrings[i];
                        component.Type = (RigidBody3DComponent::BodyType)i;
                    }

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }

                ImGui::EndCombo();
            }

            ImGui::Checkbox("Allow Sleep", &component.AllowSleep);
            ImGui::Checkbox("Awake", &component.Awake);
            
            
            ImGui::DragFloat("Mass", &component.Mass, 0.01f, 0.0f);
            ImGui::DragFloat("Linear Drag", &component.LinearDrag, 0.01f, 0.0f);
            ImGui::DragFloat("Angular Drag", &component.AngularDrag, 0.01f, 0.0f);
            ImGui::DragFloat("Gravity Scale", &component.GravityScale, 0.01f, 0.0f);
        });
        
        DrawComponent<BoxCollider3DComponent>("BoxCollider 3D", /*calling anonymous function*/ [](auto& component) {
            
            ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f);
            ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f);
            ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f);
        });
        
        
    }

    template <typename T, typename F>
    void HierarchyPanel::DrawComponent(const std::string& name, F&& lambda, const bool removable)
    {
        const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
        
        if (m_SelectedEntity)
        {
            if (m_SelectedEntity.HasComponent<T>())
            {
                auto& component = m_SelectedEntity.GetComponent<T>();
                
                bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
                
                bool removeComponent = false;
                
                if (removable)
                {
                    if (ImGui::BeginPopup("ComponentSettings"))
                    {
                        if (ImGui::MenuItem("Remove Component"))
                        {
                            removeComponent = true;
                        }
                        ImGui::EndPopup();
                    }
                    
                    float frameHeight = ImGui::GetFrameHeight();
                    ImGui::SameLine(ImGui::GetContentRegionMax().x - frameHeight * 1.2f);
                    if (ImGui::Button(ICON_MD_SETTINGS, ImVec2(frameHeight * 1.2f, frameHeight)))
                    {
                        ImGui::OpenPopup("ComponentSettings");
                    }
                }
                
                if (open)
                {
                    lambda(component);
                    ImGui::TreePop();
                }
                
                
                if (removeComponent)
                {
                    m_SelectedEntity.RemoveComponent<T>();
                }
                                
            }
        }
    }

    template <typename T>
    void HierarchyPanel::DisplayAddComponentEntry(const std::string& entryName) 
    {
        if (m_SelectedEntity)
        {
            if (!m_SelectedEntity.HasComponent<T>())
            {
                if (ImGui::MenuItem(entryName.c_str()))
                {
                    m_SelectedEntity.AddComponent<T>();
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
        
        ImGui::PushMultiItemsWidths(3, 200/*width size*/);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 4.0f, 4.0f });
        
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f });
            ImGui::Button("X");
        ImGui::PopStyleColor(2);
            ImGui::SameLine();
            ImGui::DragFloat("##X", &values.x, sliderSpeed, 0.0f, 0.0f, "%.2f");
        
        ImGui::SameLine();
        
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.8f, 0.15f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f });
            ImGui::Button("Y");
        ImGui::PopStyleColor(2);
            ImGui::SameLine();
            ImGui::DragFloat("##Y", &values.y, sliderSpeed, 0.0f, 0.0f, "%.2f");
        
        ImGui::SameLine();
        
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.15f, 0.8f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f });
            ImGui::Button("Z");
        ImGui::PopStyleColor(2);
            ImGui::SameLine();
            ImGui::DragFloat("##Z", &values.z, sliderSpeed, 0.0f, 0.0f, "%.2f");
        
        ImGui::PopItemWidth();
        ImGui::PopStyleVar();
        
        
        ImGui::PopID();
    }

}
