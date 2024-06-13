//
//  PhysicsEngine3D.hpp
//  SpectralEngine
//
//  Created by Nicolas U on 12.06.24.
//
#pragma once

// fwd declaration
namespace JPH {
    class PhysicsSystem;
    class TempAllocator;
    class JobSystemThreadPool;
}

namespace Spectral {
    
    // fwd declaration
    class BPLayerInterfaceImpl;
    class ObjectLayerPairFilterImpl;
    class ObjectVsBroadPhaseLayerFilterImpl;
    
    class PhysicsEngine3D
    {
    public:
        static void Init();
        static void Shutdown();
        static void StepPhysics(float ts);
        
        static JPH::PhysicsSystem& GetPhysicsSystem();
        
    private:
        static JPH::PhysicsSystem* s_PhysicsSystem;
        static JPH::TempAllocator* s_Allocator;
        static JPH::JobSystemThreadPool* s_JobSystemThreadPool;
        
        static BPLayerInterfaceImpl* s_BPLayerInterface;
        static ObjectLayerPairFilterImpl* s_ObjectLayerPairFilter;
        static ObjectVsBroadPhaseLayerFilterImpl* s_ObjectVSBroadPhaserLayerFilert;
    };
}

