//
//  PhysicsEngine3D.cpp
//  SpectralEngine
//
//  Created by Nicolas U on 12.06.24.
//

#include "PhysicsEngine3D.hpp"

// Jolt includes
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include "Core/Log.hpp"

namespace Spectral {

    // Layer that objects can be in, determines which other objects it can collide
    // with Typically you want at least to have 1 layer for moving bodies and 1
    // layer for static bodies
    namespace Layers {
    static constexpr JPH::ObjectLayer STATIC = 0;
    static constexpr JPH::ObjectLayer DEFAULT = 1;
    static constexpr JPH::ObjectLayer CUSTOM1 = 2;
    static constexpr JPH::ObjectLayer CUSTOM2 = 3;
    
    static constexpr JPH::ObjectLayer NUM_LAYERS = 4;
    }


    // determines if two object layers can collide
    // required by PhysicsSystem init
    class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
    {
    public:
        virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override
        {
            switch (inObject1)
            {
                case Layers::STATIC:
                    return inObject2 == Layers::STATIC;
                case Layers::DEFAULT:
                    return inObject2 == Layers::DEFAULT;
                case Layers::CUSTOM1:
                    return inObject2 == Layers::CUSTOM1;
                case Layers::CUSTOM2:
                    return inObject2 == Layers::CUSTOM2;
                default:
                    JPH_ASSERT(false);
                    return false;
            }
        }
    };
    

    // Each broadphase layer results in a separate bounding volume tree in the broad
    // phase. You at least want to have a layer for non-moving and moving objects to
    // avoid having to update a tree full of static objects every frame
    namespace BroadPhaseLayers {
        static constexpr JPH::BroadPhaseLayer STATIC(0);
        static constexpr JPH::BroadPhaseLayer DEFAULT(1);
        
        static constexpr JPH::uint NUM_LAYERS(2);
    }

    
    // BroadPhaseLayerInterface defines a mapping between object and broadphase layers
    class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
    {
    public:
        BPLayerInterfaceImpl()
        {
            // Create a mapping table from object to broad phase layer
            m_ObjectToBroadPhase[Layers::STATIC] = BroadPhaseLayers::STATIC;
            m_ObjectToBroadPhase[Layers::DEFAULT] = BroadPhaseLayers::DEFAULT;
            m_ObjectToBroadPhase[Layers::CUSTOM1] = BroadPhaseLayers::DEFAULT;
            m_ObjectToBroadPhase[Layers::CUSTOM2] = BroadPhaseLayers::DEFAULT;
        }
        
        [[nodiscard]] JPH::uint GetNumBroadPhaseLayers() const override
        {
            return BroadPhaseLayers::NUM_LAYERS;
        }
        
        [[nodiscard]] JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
        {
            JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
            return m_ObjectToBroadPhase[inLayer];
        }
        
    #if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
        virtual const char *GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override
        {
            switch ((JPH::BroadPhaseLayer::Type)inLayer)
            {
                case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::STATIC:
                    return "STATIC";
                case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::DEFAULT:
                    return "DEFAULT";
                default:
                    JPH_ASSERT(false);
                    return "INVALID";
            }
        }
    #endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED
        
    private:
        JPH::BroadPhaseLayer m_ObjectToBroadPhase[Layers::NUM_LAYERS];
    };


    // determines if an object layer can collide with a broadphase layer
    // required by PhysicsSystem init
    class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
    {
    public:
        virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override
        {
            if (inLayer1 == Layers::STATIC)
                return inLayer2 != BroadPhaseLayers::STATIC;

            return true;
        }
    };


    JPH::PhysicsSystem* PhysicsEngine3D::s_PhysicsSystem;
    JPH::TempAllocator* PhysicsEngine3D::s_Allocator;
    JPH::JobSystemThreadPool* PhysicsEngine3D::s_JobSystemThreadPool;

    BPLayerInterfaceImpl* PhysicsEngine3D::s_BPLayerInterface;
    ObjectLayerPairFilterImpl* PhysicsEngine3D::s_ObjectLayerPairFilter;
    ObjectVsBroadPhaseLayerFilterImpl* PhysicsEngine3D::s_ObjectVSBroadPhaserLayerFilert;


    
    void PhysicsEngine3D::Init()
    {
        SP_LOG_INFO("PhysicsEngine3D::Init");
        
        JPH::RegisterDefaultAllocator();
        
        JPH::Factory::sInstance = new JPH::Factory();
        
        JPH::RegisterTypes();
        
        // temp allocator for temporary allocations during the physics update
        // pre-allocating 10MB, which should be enough
        s_Allocator = new JPH::TempAllocatorImpl(10 * 1'024 * 1'024);
        
        // job system that will execute physics jobs on multiple threads
        s_JobSystemThreadPool = new JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, static_cast<int>(JPH::thread::hardware_concurrency()) - 1);
        
        constexpr JPH::uint cMaxBodies = 65536;
        constexpr JPH::uint cNumBodyMutexes = 0; // auto detect
        constexpr JPH::uint cMaxBodyPairs = 65536;
        constexpr JPH::uint cMaxContactConstraints = 10240;
        
        s_BPLayerInterface = new BPLayerInterfaceImpl();
        
        s_ObjectLayerPairFilter = new ObjectLayerPairFilterImpl();
        s_ObjectVSBroadPhaserLayerFilert = new ObjectVsBroadPhaseLayerFilterImpl();

        s_PhysicsSystem = new JPH::PhysicsSystem();
        s_PhysicsSystem->Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints,
                              *s_BPLayerInterface,
                              *s_ObjectVSBroadPhaserLayerFilert,
                              *s_ObjectLayerPairFilter);

    }

    void PhysicsEngine3D::Shutdown()
    {
        SP_LOG_INFO("PhysicsEngine3D::Shutdown");
        
        // unregisters all types with the factory and cleans up the default material
        JPH::UnregisterTypes();
        
        // free memory
        delete s_PhysicsSystem;
        delete s_Allocator;
        delete s_JobSystemThreadPool;
        delete s_BPLayerInterface;
        delete s_ObjectVSBroadPhaserLayerFilert;
        delete s_ObjectLayerPairFilter;
        delete JPH::Factory::sInstance; // destroy the factory
        
        s_PhysicsSystem = nullptr;
        s_Allocator = nullptr;
        s_JobSystemThreadPool = nullptr;
        s_BPLayerInterface = nullptr;
        s_ObjectVSBroadPhaserLayerFilert = nullptr;
        s_ObjectLayerPairFilter = nullptr;
        JPH::Factory::sInstance = nullptr;
    }

    void PhysicsEngine3D::StepPhysics(float ts)
    {
        JPH_ASSERT(s_PhysicsSystem, "Physics system not initialized");
        
        s_PhysicsSystem->Update(ts, 1, s_Allocator, s_JobSystemThreadPool);
    }

    JPH::PhysicsSystem& PhysicsEngine3D::GetPhysicsSystem()
    {
        JPH_ASSERT(s_PhysicsSystem, "Physics system not initialized");
        return *s_PhysicsSystem;
    }
}
