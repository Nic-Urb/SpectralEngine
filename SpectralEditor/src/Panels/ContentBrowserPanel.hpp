//
//  ContentBrowserPanel.hpp
//  SpectralEditor
//
//  Created by Nicolas U on 28.04.24.
//
#include "Spectral.h"

#include <filesystem>

class ContentBrowserPanel 
{
public:
    ContentBrowserPanel();
    
    void OnImGuiRender();
    
private:
    std::filesystem::path m_CurrentDir;
    
};
