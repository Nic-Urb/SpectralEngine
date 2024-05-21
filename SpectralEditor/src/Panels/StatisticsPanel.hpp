//
//  StatisticsPanel.hpp
//  SpectralEditor
//
//  Created by Nicolas U on 05.05.24.
//
#pragma once

#include "Spectral.h"

#include "raylib.h"

#include <filesystem>


namespace Spectral {

    class StatisticsPanel
    {
    public:
        StatisticsPanel() = default;
        StatisticsPanel(const std::shared_ptr<Scene>& context);
        
        void OnImGuiRender();
        
        void SetContext(const std::shared_ptr<Scene>& context);
        void SetCollisionInfo(const RayCollision& collisionInfo, const std::string& name);
        
        void SetActive(bool isActive) { m_Active = isActive; };
        bool IsActive() const { return m_Active; }
        
    private:
        std::shared_ptr<Scene> m_Context;
        RayCollision m_CollisionInfo;
        std::string m_CollisionName;
        bool m_Active = true;
        
        std::vector<float> m_Frames; // @TODO: Use for plotting, we're also need an function int GetAvgFPS(float fpsFrame)
        float m_FPSValues[100];
    };
}

