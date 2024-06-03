//
//  ContentBrowserPanel.cpp
//  SpectralEditor
//
//  Created by Nicolas U on 28.04.24.
//

#include "ContentBrowserPanel.hpp"

#include "materialdesign-main/IconsMaterialDesign.h"

#include "imgui.h"
#include "raylib.h"

ContentBrowserPanel::ContentBrowserPanel() 
    : m_CurrentDir("assets")
{
    Spectral::AssetsManager::LoadTexture("ressources/contentBrowser/directory.png");
    Spectral::AssetsManager::LoadTexture("ressources/contentBrowser/file.png");
    Spectral::AssetsManager::LoadTexture("ressources/contentBrowser/filespectral.png");
    Spectral::AssetsManager::LoadTexture("ressources/contentBrowser/filelua.png");
    Spectral::AssetsManager::LoadTexture("ressources/contentBrowser/filepng.png");
    Spectral::AssetsManager::LoadTexture("ressources/contentBrowser/file3d.png");
}

void ContentBrowserPanel::OnImGuiRender()
{
    ImGui::Begin("Content Browser");

    if (m_CurrentDir != std::filesystem::path("assets"))
    {
        if (ImGui::Button(ICON_MD_REPLY)) {
            m_CurrentDir = m_CurrentDir.parent_path();
        }
    }
    
    float padding = 14.0f;
    float thumbnailSize = 85.0f;
    float cellSize = thumbnailSize + padding;

    float panelWidth = ImGui::GetContentRegionAvail().x;
    int columnCount = (int)(panelWidth / cellSize);
    if (columnCount < 1) {
        columnCount = 1;
    }

    ImGui::Columns(columnCount, 0, false);
    
    if (std::filesystem::exists(m_CurrentDir) && std::filesystem::is_directory(m_CurrentDir))
    {
        // Loop through each item (file or subdirectory) in the directory
        for (const auto& entry : std::filesystem::directory_iterator(m_CurrentDir))
        {
            const auto& path = entry.path();
            std::string fileName = path.filename().string();
            if (fileName == ".." || fileName == "." || fileName[0] == '.') {
                continue;
            }
            
            ImGui::PushID(fileName.c_str());
            
            Texture* iconTexture = nullptr;
            if (std::filesystem::is_directory(entry))
            {
                iconTexture = Spectral::AssetsManager::GetTexture("ressources/contentBrowser/directory.png").get();
            } else {
                if (path.extension() == ".spectral") {
                    iconTexture = Spectral::AssetsManager::GetTexture("ressources/contentBrowser/filespectral.png").get();
                }
                else if (path.extension() == ".png") {
                    iconTexture = Spectral::AssetsManager::GetTexture("ressources/contentBrowser/filepng.png").get();
                }
                else if (path.extension() == ".glb" || path.extension() == ".obj" || path.extension() == ".m3d") {
                    iconTexture = Spectral::AssetsManager::GetTexture("ressources/contentBrowser/file3d.png").get();
                }
                else if (path.extension() == ".lua") {
                    iconTexture = Spectral::AssetsManager::GetTexture("ressources/contentBrowser/filelua.png").get();
                }
                else {
                    // for no supported extension just use a default file icon
                    iconTexture = Spectral::AssetsManager::GetTexture("ressources/contentBrowser/file.png").get();
                }
            }
                
            if (!iconTexture) {
                return;
            }
                
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
            ImGui::ImageButton(fileName.c_str(), (ImTextureID)iconTexture, ImVec2(thumbnailSize, thumbnailSize), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(0.0f, 0.0f, 0.0f, 0.0f), ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
            ImGui::PopStyleColor();
            
            if (ImGui::BeginDragDropSource())
            {
                const char* dragType = nullptr;
                
                if (path.extension() == ".spectral") {
                    dragType = "SCENE_PAYLOAD";
                }
                else if (path.extension() == ".png") {
                    dragType = "TEXTURE_PAYLOAD";
                }
                else if (path.extension() == ".glb" || path.extension() == ".obj" || path.extension() == ".m3d") {
                    dragType = "3D_PAYLOAD";
                }
                else if (path.extension() == ".lua") {
                    dragType = "SCRIPT_PAYLOAD";
                }
                
                if (dragType)
                {
                    std::filesystem::path relativePath(path);
                    const char* itemPath = relativePath.c_str();
                    
                    ImGui::SetDragDropPayload(dragType, itemPath, (strlen(itemPath) + 1) * sizeof(char));
                }
                
                ImGui::Text("%s",fileName.c_str());
                ImGui::Image((ImTextureID)iconTexture, { thumbnailSize, thumbnailSize });
                
                ImGui::EndDragDropSource();
            }
                
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                if (entry.is_directory()) {
                    m_CurrentDir /= path.filename();
                }
            }
            
            ImGui::TextWrapped("%s", fileName.c_str());
                
            ImGui::NextColumn();
            ImGui::PopID();
        }
        ImGui::Columns(1);
        ImGui::End();
    }
    
}
