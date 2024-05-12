//
//  Panel.hpp
//  SpectralEditor
//
//  Created by Nicolas U on 05.05.24.
//
#pragma once

#include "Spectral.h"

class Panel
{
public:
    Panel(const std::string& name = "Unnamed Panel", bool isActive = false);
    virtual ~Panel() = default;
    
    virtual void OnImGuiRender() = 0;
    
    void SetActive(bool isActive) { m_Active = isActive; };
    bool IsActive() const { return m_Active; }

    const std::string& GetName() { return m_Name; }
    
private:
    bool m_Active;
    std::string m_Name;
};
