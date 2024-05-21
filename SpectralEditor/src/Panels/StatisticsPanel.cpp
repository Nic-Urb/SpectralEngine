//
//  StatisticsPanel.cpp
//  SpectralEditor
//
//  Created by Nicolas U on 05.05.24.
//
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include "StatisticsPanel.hpp"

#include "imgui.h"
#include "raylib.h"
#include "OpenGL/gl.h"

namespace Spectral {

    StatisticsPanel::StatisticsPanel(const std::shared_ptr<Scene>& context)
        : m_Context(context)
    {}
    
    void StatisticsPanel::OnImGuiRender()
    {
        ImGui::Begin("Engine Stats");
        
        if (ImGui::BeginTabBar("##Stats"))
        {
            if (ImGui::BeginTabItem("General"))
            {
                ImGui::Text("Loaded Textures: %zu", TextureManager::GetLoadedTextureCount());
                ImGui::Text("Loaded Objects: %zu", m_Context->GetObjectCount());
                
                ImGui::Separator();
                
                // @TODO: Add: "Build: VERSION (__TIME__) (__DATE__) Debug/Release"
                
                ImGui::Text("OpenGL Device Information: ");
                ImGui::Spacing();
                ImGui::Text("Vendor: %s", glGetString(GL_VENDOR));
                ImGui::Text("Renderer: %s", glGetString(GL_RENDERER));
                ImGui::Text("Version: %s", glGetString(GL_VERSION));
                ImGui::Text("GLSL: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
                
                ImGui::Separator();
                
                // @TODO: Get version from other libs: FMOD, Box2D, PhysX...
                ImGui::Text("Libs compiled versions: ");
                ImGui::Spacing();
                ImGui::Text("ImGui: %s", ImGui::GetVersion());
                ImGui::Text("RayLib: %s", RAYLIB_VERSION);
                
                
                ImGui::EndTabItem();
            }
            
            if (ImGui::BeginTabItem("Debug"))
            {                
                ImGui::Text("Mouse Hit Info: ");
                ImGui::Spacing();
                ImGui::Text("Hit Object: %s", m_CollisionName.c_str());
                ImGui::Text("Ray Cast Distance: %f", m_CollisionInfo.distance);
                ImGui::Text("Hit Pos: %.2f %.2f %.2f",
                            m_CollisionInfo.point.x,
                            m_CollisionInfo.point.y,
                            m_CollisionInfo.point.z);
                ImGui::Text("Hit Norm: %.2f %.2f %.2f",
                            m_CollisionInfo.normal.x,
                            m_CollisionInfo.normal.y,
                            m_CollisionInfo.normal.z);
                ImGui::EndTabItem();
            }
            
            if (ImGui::BeginTabItem("Performance"))
            {
                Application& app = Application::GetInstance();
                
                const float fps = 1/(float)app.GetTimestep();
                ImColor frametimeColor;
                
                if (fps <= 20.0f) {
                    frametimeColor = IM_COL32(190, 2, 2, 255); // RED
                } else if (fps < 28.0f) {
                    frametimeColor = IM_COL32(231, 150, 0, 255); // ORANGE
                } else {
                    frametimeColor = IM_COL32(32, 207, 0, 255); // GREEN
                }
                
                ImGui::PushStyleColor(ImGuiCol_Text, frametimeColor.Value);
                ImGui::Text("Client FPS: %.1f (%.2f ms)", fps, app.GetTimestep().GetMiliseconds());
                ImGui::PopStyleColor();
                
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        
        
        ImGui::End();
    }
    
    void StatisticsPanel::SetContext(const std::shared_ptr<Scene> &context)
    {
        m_Context = context;
    }
    
    void StatisticsPanel::SetCollisionInfo(const RayCollision &collisionInfo, const std::string& name)
    {
        m_CollisionInfo = collisionInfo;
        m_CollisionName = name;
    }

}

