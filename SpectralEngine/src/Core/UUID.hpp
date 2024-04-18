//
//  UUID.hpp
//  SpectralEngine
//
//  Created by Nicolas U on 05.04.24.
//
#pragma once

#include "pch.h"


namespace Spectral {
    
    // calling this creates universal unique identfier
    class UUID
    {
    public:
        UUID();
        
        operator uint64_t() const { return m_UUID; }
        
    private:
        uint64_t m_UUID;
        
    };
}

// specialization for hash c++ std function to support UUID
namespace std {

    template<>
    struct hash<Spectral::UUID>
    {
        std::size_t operator()(const Spectral::UUID& uuid) const
        {
            return hash<uint64_t>{}((uint64_t)uuid);
        }
    };
}
