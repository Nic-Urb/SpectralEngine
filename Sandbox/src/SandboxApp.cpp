//
//  SandboxApp.cpp
//  Sandbox
//
//  Created by Nicolas U on 04.04.24.
//
#include "Spectral.h"
#include "Core/Entrypoint.cpp"

#include "Renderer/EditorCamera.hpp"
#include "rlImGui.h"
#include "imgui.h"

extern Spectral::Application* Spectral::CreateApplication(); // don't think we need this here

class SandboxLayer : public Spectral::Layer
{
public:
    SandboxLayer()
    {
        m_SandboxWorld = std::make_shared<Spectral::Scene>();
        
        m_EditorCamera = Spectral::EditorCamera();
        
        //auto a = m_SandboxWorld->CreateObject<Spectral::StaticObject>();
        //auto b = m_SandboxWorld->CreateObject<Spectral::StaticObject>();
        
        Spectral::TextureManager::LoadTexture("assets/textures/Checkerboard.png");
        
        //auto comp1 = a->GetComponent<Spectral::SpriteComponent>();
        //comp1->SetSprite(Spectral::TextureManager::GetTexture("assets/textures/Checkerboard.png"));
        
        //a->SetName("Static object 1");
        //b->SetName("Static object 2");
        
        //SP_CLIENT_LOG_INFO("Object uuid: ({0})", a->GetUUID());
        //SP_CLIENT_LOG_INFO("Object uuid: ({0})", b->GetUUID());
        
        m_Framebuffer = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
        SetTextureFilter(m_Framebuffer.texture, TEXTURE_FILTER_BILINEAR);
    }
    
    ~SandboxLayer() override
    {
        UnloadRenderTexture(m_Framebuffer);
    }
    
    void OnAttach() override
    {
        SP_CLIENT_LOG_INFO("SANDBOX-APP::OnAttach");
        // On attach is never reached !!
    }
    
    void OnUpdate(Spectral::Timestep ts) override
    {
        m_EditorCamera.OnUpdate(ts);
        //SP_CLIENT_LOG_TRACE("Delta time: {0}s, ({1})", ts, ts.GetMiliseconds());
        m_SandboxWorld->OnUpdateEditor(ts, m_EditorCamera); // call OnRuntimeUpdate instead
        
        // draw texture internally before the drawing phase
        BeginTextureMode(m_Framebuffer);
            ClearBackground(BLACK); // swap buffers
            m_SandboxWorld->OnRenderEditor(m_EditorCamera); // call OnRuntimeRender instead
        EndTextureMode();
    }
    
    void OnRender() override
    {
       // draw the rendered texture onto the screen
       DrawTexturePro(m_Framebuffer.texture, (Rectangle){ 0, 0, (float)m_Framebuffer.texture.width, (float)-m_Framebuffer.texture.height }, (Rectangle){ 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() }, (Vector2){ 0, 0 }, 0, WHITE);
        // @TODO: Implment anti aliasing to handle unsharp edges
    }
    
private:
    std::shared_ptr<Spectral::Scene> m_SandboxWorld;
    Spectral::EditorCamera m_EditorCamera;
    
    RenderTexture m_Framebuffer;
};

class SandboxApp : public Spectral::Application
{
public:
    
    SandboxApp() : Spectral::Application("[Spectral Sandbox - Applicaton]")
    {
        PushLayer(new SandboxLayer());
        SP_CLIENT_LOG_INFO("SANDBOX-APP::Run");
    }
};

Spectral::Application* Spectral::CreateApplication()
{
    return new SandboxApp();
}
