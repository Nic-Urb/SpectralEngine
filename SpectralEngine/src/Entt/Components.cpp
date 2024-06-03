//
//  Components.cpp
//  SpectralEngine
//
//  Created by Nicolas U on 01.06.24.
//

#include "Components.hpp"

#include "rlgl.h"

namespace Spectral {

// ----- IDComponent -----
    
    IDComponent::IDComponent(UUID uuid, const std::string& name)
            : ID(uuid), Name(name)
    {}

// ----- TransformComponent -----

    void TransformComponent::PushMatrix()
    {
        rlPushMatrix();
        rlTranslatef(Translation.x, Translation.y, Translation.z);
        rlRotatef(Rotation.x, 1, 0, 0);
        rlRotatef(Rotation.y, 0, 1, 0);
        rlRotatef(Rotation.z, 0, 0, 1);
        rlScalef(Scale.x, Scale.y, Scale.z);
    }

    void TransformComponent::PopMatrix()
    {
        rlPopMatrix();
    }

// ----- CameraComponent -----

    CameraComponent::CameraComponent()
    {
        Camera = std::make_shared<RuntimeCamera>();
    }
}
