//
//  TextureManager.cpp
//  SpectralEngine
//
//  Created by Nicolas U on 07.04.24.
//

#include "TextureManager.hpp"


namespace Spectral {

    std::unordered_map<std::string, Texture> TextureManager::m_TexturesRegistry;

    void TextureManager::LoadTexture(const std::string& texturePath)
    {
        if (TextureExists(texturePath)) {
            SP_LOG_WARN("LoadTexture::Texture at path ({0}) exist!", texturePath);
            return;
        }
        
        const Texture texture = ::LoadTexture(texturePath.c_str());
        if (texture.id > 0) {
            m_TexturesRegistry[texturePath] = texture;
        } else {
            SP_LOG_WARN("LoadTexture::Texture ({0}) has failed to load, we can't add them to registry.", texturePath);
        }
    }

    void TextureManager::UnloadTexture(const std::string& texturePath)
    {
        if (!TextureExists(texturePath)) {
            SP_LOG_WARN("UnloadTexture::Texture at path ({0}) does not exist!", texturePath);
            return;
        }
        
        ::UnloadTexture(m_TexturesRegistry[texturePath]);
        m_TexturesRegistry.erase(texturePath);
    }

    void TextureManager::UnloadAllTextures()
    {
        for (const auto& [_, v] : m_TexturesRegistry)
        {
            ::UnloadTexture(v);
        }
        m_TexturesRegistry.clear();
    }

    Texture TextureManager::GetTexture(const std::string &texturePath)
    {
        if (TextureExists(texturePath)) {
            return m_TexturesRegistry[texturePath];
        }
        
        SP_LOG_WARN("GetTetxture::Texture does not exist, can't access the texture!");
        return {};
    }

    bool TextureManager::TextureExists(const std::string& texturePath)
    {
        return m_TexturesRegistry.find(texturePath) != m_TexturesRegistry.end();
    }

}
