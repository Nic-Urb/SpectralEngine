//
//  HashString.cpp
//  SpectralEngine
//
//  Created by Nicolas U on 02.04.24.
//

#include "pch.h"

struct Hash {
    // compile time "hash" function that converts a string
    static constexpr uint32_t HashString(std::string_view str)
    {
        uint32_t id{};
        for (char c : str)
        {
            id = (id * 31) + static_cast<uint32_t>(c);
        }
        return id;
    }
};

