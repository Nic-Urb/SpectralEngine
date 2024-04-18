//
//  CameraController.hpp
//  SpectralEngine
//
//  Created by Nicolas U on 04.04.24.
//

#include "pch.h"
#include "raylib.h"
#include "Camera.h"

namespace Spectral {
    
    // controls runtime camera
    class CameraController
    {
    public:
        CameraController() {}
        
        void FocusCameraToObject() {} // moves camera to some object position and focus to their location/position
        void BlendCameras(Camera& otherCamera) {} // blends into the other camera using interpolator
    
    private:
        Camera2D m_Camera = { 0 };
        
    private:
        
        
    };
}
