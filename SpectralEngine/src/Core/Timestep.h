//
//  Timestep.h
//  SpectralEngine
//
//  Created by Nicolas U on 04.04.24.
//
#pragma once

namespace Spectral {

    struct Timestep
    {
        Timestep(float time = 0.0f) : m_Time(time) {}
        
        operator float() const { return m_Time; };
        float GetMiliseconds() const { return m_Time * 1000.0f;};
        
    private:
        float m_Time;
        
    };

}
