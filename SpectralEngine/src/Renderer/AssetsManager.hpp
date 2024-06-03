//
//  AssetsManager.hpp
//  SpectralEngine
//
//  Created by Nicolas U on 07.04.24.
//
#pragma once

#include "pch.h"
#include "raylib.h"

namespace Spectral {

    // The primary concept behind the asset manager is to rather than loading the same texture multiple times, we load them only once, to save on memory.
    class AssetsManager // @TODO: Make assets manager generic
    {
    public:
        static std::shared_ptr<Texture> LoadTexture(const std::string& texturePath);
        static std::shared_ptr<Model> LoadModel(const std::string& modelPath);
        
        static void UnloadTexture(const std::string& texturePath);
        static void UnloadModel(const std::string& modelPath);
        
        static void UnloadAllAssets(); // @TODO: Call this in client's Layer
        
        static std::shared_ptr<Texture> GetTexture(const std::string& texturePath);
        static std::shared_ptr<Model> GetModel(const std::string& modelPath);
        
        static std::string GetTexturePath(const Texture& texture);
        static std::string GetModelPath(const Model& model);
        
        static bool TextureExists(const std::string& texturePath);
        static bool ModelExists(const std::string& modelPath);
        
        static size_t GetLoadedAssetsCount() { return m_TexturesRegistry.size() + m_ModelRegistry.size(); }
        
        /* @TODO:
         Stats {
            LoadedTextures;
            LoadedModels;
            ....
         }
         */

    private:
        static std::unordered_map<std::string, std::shared_ptr<Texture>> m_TexturesRegistry;
        static std::unordered_map<std::string, std::shared_ptr<Model>>   m_ModelRegistry;
        
    };
}
