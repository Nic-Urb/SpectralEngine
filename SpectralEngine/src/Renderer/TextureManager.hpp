//
//  TextureManager.hpp
//  SpectralEngine
//
//  Created by Nicolas U on 07.04.24.
//
#pragma once

#include "pch.h"
#include "raylib.h"

namespace Spectral {

    // The primary concept behind the asset manager is to rather than loading the same texture multiple times, we load them only once, to save on memory.
    class TextureManager
    {
    public:
        static void LoadTexture(const std::string& texturePath);
        static void UnloadTexture(const std::string& texturePath);
        static void UnloadAllTextures(); // @TODO: Call this in client's Layer
        
        static Texture GetTexture(const std::string& texturePath);
        
        static bool TextureExists(const std::string& texturePath);
        
    private:
        static std::unordered_map<std::string, Texture> m_TexturesRegistry;
        
    };
}
