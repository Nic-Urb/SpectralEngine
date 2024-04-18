//
//  Layer.hpp
//  SpectralEngine
//
//  Created by Nicolas U on 20.03.24.
//
#pragma once

#include "pch.h"
#include "Timestep.h"

namespace Spectral {
    
    class Layer
    {
    public:
        
        Layer(const std::string& name = "Layer");
        virtual ~Layer();
        
        virtual void OnAttach() {}
        virtual void OnDetach() {}
        
        virtual void OnUpdate(Timestep ts) {}
        virtual void OnRender() {}
        virtual void OnImGuiRender() {}
        
        const std::string& GetName() const
        {
            return m_DebugName;
        }
        
    protected:
        
        std::string m_DebugName;
        
    };
}
