//
//  StatisticsPanel.cpp
//  SpectralEditor
//
//  Created by Nicolas U on 05.05.24.
//

#include "StatisticsPanel.hpp"

#include "imgui.h"
#include "raylib.h"

StatisticsPanel::StatisticsPanel(const std::string& name)
    : Panel(name, true)
{}

void StatisticsPanel::OnImGuiRender()
{
    ImGui::Begin("Engine Stats");
    
    if (ImGui::BeginTabBar("##Stats"))
    {
        if (ImGui::BeginTabItem("General"))
        {
            ImGui::Text("Loaded Textures: %zu", Spectral::TextureManager::GetLoadedTextureCount());
            
            ImGui::Separator();
            
            // @TODO: Add: "Build: VERSION (__TIME__) (__DATE__) Debug/Release"
            // @TODO: Get version from other libs: FMOD, Box2D, PhysX...
            
            ImGui::Text("Libs compiled versions: ");
            ImGui::Spacing();
            ImGui::Text("ImGui: %s", ImGui::GetVersion());
            ImGui::Text("RayLib: %s", RAYLIB_VERSION);
            
            
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("Performance"))
        {
            Spectral::Application& app = Spectral::Application::GetInstance();
            
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
