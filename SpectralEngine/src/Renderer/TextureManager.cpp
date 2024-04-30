//
//  TextureManager.cpp
//  SpectralEngine
//
//  Created by Nicolas U on 07.04.24.
//
#include "TextureManager.hpp"


namespace Spectral {

    std::unordered_map<std::string, std::shared_ptr<Texture>> TextureManager::m_TexturesRegistry;

    std::shared_ptr<Texture> TextureManager::LoadTexture(const std::string& texturePath)
    {
        if (TextureExists(texturePath)) {
            SP_LOG_WARN("LoadTexture::Texture at path ({0}) exist!", texturePath);
            return m_TexturesRegistry[texturePath];
        }
        
        const Texture texture = ::LoadTexture(texturePath.c_str());
        if (texture.id > 0) {
            std::shared_ptr<Texture> texturePtr = std::make_shared<Texture>(texture);
            m_TexturesRegistry[texturePath] = texturePtr;
            return texturePtr;
        } else {
            SP_LOG_WARN("LoadTexture::Texture ({0}) has failed to load, we can't add them to registry.", texturePath);
            return nullptr;
        }
    }

    void TextureManager::UnloadTexture(const std::string& texturePath)
    {
        if (!TextureExists(texturePath)) {
            SP_LOG_WARN("UnloadTexture::Texture at path ({0}) does not exist!", texturePath);
            return;
        }
        
        ::UnloadTexture(*m_TexturesRegistry[texturePath]);
        m_TexturesRegistry.erase(texturePath);
    }

    void TextureManager::UnloadAllTextures()
    {
        for (const auto& [_, v] : m_TexturesRegistry)
        {
            ::UnloadTexture(*v);
        }
        m_TexturesRegistry.clear();
    }

    std::shared_ptr<Texture> TextureManager::GetTexture(const std::string &texturePath)
    {
        if (TextureExists(texturePath)) {
            return m_TexturesRegistry[texturePath];
        }
        
        SP_LOG_WARN("GetTetxture::Texture does not exist, can't access the texture!");
        return nullptr;
    }

    bool TextureManager::TextureExists(const std::string& texturePath)
    {
        return m_TexturesRegistry.find(texturePath) != m_TexturesRegistry.end();
    }

}
