//
//  StatisticsPanel.hpp
//  SpectralEditor
//
//  Created by Nicolas U on 05.05.24.
//
#include "Panel.hpp"

#include <filesystem>

class StatisticsPanel : public Panel
{
public:
    StatisticsPanel(const std::string& name = "Engine Stats");
    
    void OnImGuiRender();
    
private:
    std::vector<float> m_Frames; // @TODO: Use for plotting, we're also need an function int GetAvgFPS(float fpsFrame)
    float m_FPSValues[100];
};
