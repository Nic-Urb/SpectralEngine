//
//  EntryPoint.cpp
//  SpectralEngine
//
//  Created by Nicolas U on 24.03.24.
//

#include "Application.hpp"

int main(int argc, const char * argv[]) {
    Spectral::Log::init();
    
    auto* app = Spectral::CreateApplication();
    if (app) {
        app->Run();
    }
    delete app;
}
