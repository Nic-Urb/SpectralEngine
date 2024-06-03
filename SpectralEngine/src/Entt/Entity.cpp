//
//  EnttObject.cpp
//  SpectralEngine
//
//  Created by Nicolas U on 01.06.24.
//

#include "Entity.hpp"

namespace Spectral {
    Entity::Entity(entt::entity handle, Scene* context)
            : m_EnttHandle(handle), m_SceneContext(context)
    {}
}
