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
        static std::shared_ptr<Texture> LoadTexture(const std::string& texturePath);
        static void UnloadTexture(const std::string& texturePath);
        static void UnloadAllTextures(); // @TODO: Call this in client's Layer
        
        static std::shared_ptr<Texture> GetTexture(const std::string& texturePath);
        static std::string GetTexturePath(const Texture& texture);
        
        static bool TextureExists(const std::string& texturePath);
        
        static size_t GetLoadedTextureCount() { return m_TexturesRegistry.size(); }
        
        /* @TODO:
         Stats {
            LoadedTextures;
            LoadedModels;
            ....
         }
         */

    private:
        static std::unordered_map<std::string, std::shared_ptr<Texture>> m_TexturesRegistry;
        
    };
}
