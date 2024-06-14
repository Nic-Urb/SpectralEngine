//
//  EditorApp.cpp
//  SpectralEditor
//
//  Created by Nicolas U on 04.04.24.
//

#include "Spectral.h"
#include "Core/EntryPoint.cpp"

#include "EditorLayer.hpp"

extern Spectral::Application* Spectral::CreateApplication();

class EditorApp : public Spectral::Application
{
public:
    
    EditorApp() : Spectral::Application()
    {
        PushLayer(new Spectral::EditorLayer());
        SP_CLIENT_LOG_INFO("EDITOR-APP::Run");
    }
    
    ~EditorApp()
    {
        SP_CLIENT_LOG_INFO("EDITOR-APP::Shutdown");
    }
};

Spectral::Application* Spectral::CreateApplication()
{
    return new EditorApp();
}
