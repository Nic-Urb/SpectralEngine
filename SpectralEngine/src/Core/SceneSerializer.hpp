//
//  SceneSerializer.hpp
//  SpectralEngine
//
//  Created by Nicolas U on 30.04.24.
//
#pragma once

#include "Scene.hpp"

namespace Spectral {

    class SceneSerializer
    {
    public:
        SceneSerializer(const std::shared_ptr<Scene>& scene);
        
        void Serialize(const std::string& filePath);
        bool Deserialize(const std::string& filePath);
    
    private:
        std::shared_ptr<Scene> m_Scene;
        
    };
}
