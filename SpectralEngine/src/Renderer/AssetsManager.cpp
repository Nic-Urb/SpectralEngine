//
//  AssetsManager.cpp
//  SpectralEngine
//
//  Created by Nicolas U on 07.04.24.
//
#include "AssetsManager.hpp"


namespace Spectral {

    std::unordered_map<std::string, std::shared_ptr<Texture>> AssetsManager::m_TexturesRegistry;
    std::unordered_map<std::string, std::shared_ptr<Model>> AssetsManager::m_ModelRegistry;

    std::shared_ptr<Texture> AssetsManager::LoadTexture(const std::string& texturePath)
    {
        if (TextureExists(texturePath)) {
            SP_LOG_WARN("LoadTexture::Texture at path ({0}) exist!", texturePath);
            return m_TexturesRegistry[texturePath];
        }
        
        const Texture& texture = ::LoadTexture(texturePath.c_str());
        if (texture.id > 0) {
            std::shared_ptr<Texture> texturePtr = std::make_shared<Texture>(texture);
            m_TexturesRegistry[texturePath] = texturePtr;
            return texturePtr;
        } else {
            SP_LOG_WARN("LoadTexture::Texture ({0}) has failed to load, we can't add them to registry.", texturePath);
            return nullptr;
        }
    }

    std::shared_ptr<Model> AssetsManager::LoadModel(const std::string& modelPath)
    {
        if (ModelExists(modelPath)) {
            SP_LOG_WARN("LoadModel::Model at path ({0}) exist!", modelPath);
            return m_ModelRegistry[modelPath];
        }
        
        const Model& model = ::LoadModel(modelPath.c_str());
        if (IsModelReady(model)) {
            std::shared_ptr<Model> modelPtr = std::make_shared<Model>(model);
            m_ModelRegistry[modelPath] = modelPtr;
            return modelPtr;
        } else {
            SP_LOG_WARN("LoadModel::Model ({0}) has failed to load, we can't add them to registry.", modelPath);
            return nullptr;
        }
    }

    void AssetsManager::UnloadTexture(const std::string& texturePath)
    {
        if (!TextureExists(texturePath)) {
            SP_LOG_WARN("UnloadTexture::Texture at path ({0}) does not exist!", texturePath);
            return;
        }
        
        ::UnloadTexture(*m_TexturesRegistry[texturePath]);
        m_TexturesRegistry.erase(texturePath);
    }

    void AssetsManager::UnloadModel(const std::string& modelPath)
    {
        if (!ModelExists(modelPath)) {
            SP_LOG_WARN("UnloadModel::Model at path ({0}) does not exist!", modelPath);
            return;
        }
        
        ::UnloadModel(*m_ModelRegistry[modelPath]);
        m_ModelRegistry.erase(modelPath);
    }

    void AssetsManager::UnloadAllAssets()
    {
        for (const auto& [_, v] : m_TexturesRegistry)
        {
            ::UnloadTexture(*v);
        }
        
        for (const auto& [_, v] : m_ModelRegistry)
        {
            ::UnloadModel(*v);
        }
        
        m_TexturesRegistry.clear();
        m_ModelRegistry.clear();
    }

    std::shared_ptr<Texture> AssetsManager::GetTexture(const std::string &texturePath)
    {
        if (TextureExists(texturePath)) {
            return m_TexturesRegistry[texturePath];
        }
        
        SP_LOG_WARN("GetTetxture::Texture does not exist, can't access the texture!");
        return nullptr;
    }

    std::shared_ptr<Model> AssetsManager::GetModel(const std::string &modelPath)
    {
        if (ModelExists(modelPath)) {
            return m_ModelRegistry[modelPath];
        }
        
        SP_LOG_WARN("GetModel::Model does not exist, can't access the model!");
        return nullptr;
    }

    std::string AssetsManager::GetTexturePath(const Texture& texture)
    {
        auto it = std::find_if(m_TexturesRegistry.begin(), m_TexturesRegistry.end(),
                                   [&texture](const auto& v) { return v.second->id == texture.id; });

        if (it == m_TexturesRegistry.end())
        {
            return "";
        }
                
        return it->first;
    }

    std::string AssetsManager::GetModelPath(const Model &model)
    {
        auto it = std::find_if(m_ModelRegistry.begin(), m_ModelRegistry.end(),
                               [&model](const auto& v) { return v.second->meshes == model.meshes && v.second->meshCount == model.meshCount; });

        if (it == m_ModelRegistry.end())
        {
            return "";
        }
                
        return it->first;
    }

    bool AssetsManager::TextureExists(const std::string& texturePath)
    {
        return m_TexturesRegistry.find(texturePath) != m_TexturesRegistry.end();
    }

    bool AssetsManager::ModelExists(const std::string &modelPath)
    {
        return m_ModelRegistry.find(modelPath) != m_ModelRegistry.end();
    }
}
