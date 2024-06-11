//
//  SceneSerializer.cpp
//  SpectralEngine
//
//  Created by Nicolas U on 30.04.24.
//

#include "SceneSerializer.hpp"

#include "yaml-cpp/yaml.h"

#include "Entt/Entity.hpp"
#include "Entt/Components.hpp"
#include "Renderer/AssetsManager.hpp"
#include "Core/UUID.hpp"

#include <fstream>


/* YAML */
namespace YAML {

    template<>
    struct convert<Vector3>
    {
        static Node encode(const Vector3& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }

        static bool decode(const Node& node, Vector3& rhs)
        {
            if(!node.IsSequence() || node.size() != 3) {
                return false;
            }

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };

    template<>
    struct convert<Vector4>
    {
        static Node encode(const Vector4& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            return node;
        }

        static bool decode(const Node& node, Vector4& rhs)
        {
            if(!node.IsSequence() || node.size() != 4) {
                return false;
            }

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };
    
    template<>
    struct convert<Spectral::UUID>
    {
        static Node encode(const Spectral::UUID& rhs)
        {
            Node node;
            node.push_back((uint64_t)rhs);
            return node;
        }

        static bool decode(const Node& node, Spectral::UUID& rhs)
        {
            rhs = node.as<uint64_t>();
            return true;
        }
};
}

namespace Spectral {

    YAML::Emitter& operator<<(YAML::Emitter& out, const Vector2& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const Vector3& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const Vector4& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
        return out;
    }

    static std::string RigidBody2DBodyTypeToString(RigidBody2DComponent::BodyType bodyType)
    {
        switch (bodyType)
        {
            case RigidBody2DComponent::BodyType::Static:    return "Static";
            case RigidBody2DComponent::BodyType::Kinematic: return "Kinematic";
            case RigidBody2DComponent::BodyType::Dynamic:   return "Dynamic";
        }

        // @TODO: ASSERT(false, "Unknown body type");
        return {};
    }

    static RigidBody2DComponent::BodyType RigidBody2DBodyTypeFromString(const std::string& bodyTypeString)
    {
        if (bodyTypeString == "Static")    return RigidBody2DComponent::BodyType::Static;
        if (bodyTypeString == "Kinematic") return RigidBody2DComponent::BodyType::Kinematic;
        if (bodyTypeString == "Dynamic")   return RigidBody2DComponent::BodyType::Dynamic;
    
        // @TODO: ASSERT(false, "Unknown body type");
        return RigidBody2DComponent::BodyType::Static;
    }

    static void SerializeEntity(YAML::Emitter& out, Entity entt)
    {
        out << YAML::BeginMap; // Object
        
        out << YAML::Key << "ID" << YAML::Value << entt.GetUUID();
        out << YAML::Key << "Name" << YAML::Value << entt.GetName();
        
        if (entt.HasComponent<TransformComponent>())
        {
            out << YAML::Key << "TransformComponent";
            out << YAML::BeginMap; // TransformComponent
            
            auto& tc = entt.GetComponent<TransformComponent>();
            out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
            out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
            out << YAML::Key << "Scale" << YAML::Value << tc.Scale;
            
            out << YAML::EndMap; // TransformComponent
        }
        
        if (entt.HasComponent<SpriteComponent>())
        {
            out << YAML::Key << "SpriteComponent";
            out << YAML::BeginMap; // SpriteComponent
            
            auto& sc = entt.GetComponent<SpriteComponent>();
            out << YAML::Key << "Texture" << YAML::Value << AssetsManager::GetTexturePath(sc.SpriteTexture);
            out << YAML::Key << "Tint" << YAML::Value << sc.Tint;
            
            out << YAML::EndMap; // SpriteComponent
        }
        
        if (entt.HasComponent<ModelComponent>())
        {
            out << YAML::Key << "ModelComponent";
            out << YAML::BeginMap; // ModelComponent
            
            auto& mc = entt.GetComponent<ModelComponent>();
            out << YAML::Key << "Model" << YAML::Value << AssetsManager::GetModelPath(mc.ModelData);
            // @TODO: Serialize/Deserialize all data for model
            
            out << YAML::EndMap; // ModelComponent
        }
        
        if (entt.HasComponent<RigidBody2DComponent>())
        {
            out << YAML::Key << "RigidBody2DComponent";
            out << YAML::BeginMap; // RigidBody2DComponent
            
            auto& rb2c = entt.GetComponent<RigidBody2DComponent>();
            out << YAML::Key << "Type" << YAML::Value << RigidBody2DBodyTypeToString(rb2c.Type);
            out << YAML::Key << "FixedRotation" << YAML::Value << rb2c.FixedRotation;
            out << YAML::Key << "AllowSleep" << YAML::Value << rb2c.AllowSleep;
            out << YAML::Key << "Awake" << YAML::Value << rb2c.Awake;
            out << YAML::Key << "GravityScale" << YAML::Value << rb2c.GravityScale;
            
            out << YAML::EndMap; // RigidBody2DComponent
        }
        
        if (entt.HasComponent<BoxCollider2DComponent>())
        {
            out << YAML::Key << "BoxCollider2DComponent";
            out << YAML::BeginMap; // BoxCollider2DComponent
            
            auto& bc2d = entt.GetComponent<BoxCollider2DComponent>();
            out << YAML::Key << "Density" << YAML::Value << bc2d.Density;
            out << YAML::Key << "Friction" << YAML::Value << bc2d.Friction;
            out << YAML::Key << "Restitution" << YAML::Value << bc2d.Restitution;
            
            out << YAML::EndMap; // BoxCollider2DComponent
        }
        
        if (entt.HasComponent<CameraComponent>())
        {
            out << YAML::Key << "CameraComponent";
            out << YAML::BeginMap; // CameraComponent
            
            auto& cc = entt.GetComponent<CameraComponent>();
            out << YAML::Key << "Active" << YAML::Value << cc.Active;
            out << YAML::Key << "Projection" << YAML::Value << cc.Camera->GetCamera3D().projection;
            out << YAML::Key << "FOV" << YAML::Value << cc.Camera->GetCamera3D().fovy;
            
            out << YAML::EndMap; // CameraComponent
        }
        
        if (entt.HasComponent<LuaScriptComponent>())
        {
            out << YAML::Key << "LuaScriptComponent";
            out << YAML::BeginMap; // LuaScriptComponent
            
            auto& lsc = entt.GetComponent<LuaScriptComponent>();
            out << YAML::Key << "Path" << YAML::Value << lsc.ScriptPath;
            
            out << YAML::EndMap; // LuaScriptComponent
        }
        
        out << YAML::EndMap; // Object
    }

// SceneSerializer class

    SceneSerializer::SceneSerializer(const std::shared_ptr<Scene>& scene)
        : m_Scene(scene)
    {}

    void SceneSerializer::Serialize(const std::string& filePath)
    {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene" << YAML::Value << m_Scene->m_Name;
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
        
        for(auto entityID : m_Scene->m_Registry.view<entt::entity>())
        {
            Entity entity = { entityID, m_Scene.get() };
            
            if (!entity) {
                return;
            }
            
            SerializeEntity(out, entity);
        }
        
        out << YAML::EndSeq;
        out << YAML::EndMap;
        
        // output file
        std::ofstream outfile(filePath);
        outfile << out.c_str(); // Write YAML output to file
    }

    bool SceneSerializer::Deserialize(const std::string& filePath)
    {
        YAML::Node data;
        
        try {
            data = YAML::LoadFile(filePath);
        } catch (YAML::ParserException exc) {
            // Log some error
            return false;
        }
        
        if (!data["Scene"]) {
            return false;
        }
        
        auto entities = data["Entities"];
        if (entities)
        {
            for (auto entity : entities)
            {
                uint64_t uuid = entity["ID"].as<Spectral::UUID>();
                const std::string& name = entity["Name"].as<std::string>();
                
                auto entt = m_Scene->CreateEntity(uuid, name);
                
                SP_LOG_INFO("Deserializing object : {0}, {1}", uuid, name);
                
                auto transformComponent = entity["TransformComponent"];
                if (transformComponent)
                {
                    auto& tc = entt.GetOrAddComponent<TransformComponent>();
                    tc.Translation = transformComponent["Translation"].as<Vector3>();
                    tc.Rotation = transformComponent["Rotation"].as<Vector3>();
                    tc.Scale = transformComponent["Scale"].as<Vector3>();
                }
                
                auto spriteComponent = entity["SpriteComponent"];
                if (spriteComponent)
                {
                    auto& sc = entt.GetOrAddComponent<SpriteComponent>();
                    auto texturePath = spriteComponent["Texture"].as<std::string>();
                    if (!texturePath.empty()) {
                        sc.SpriteTexture = *AssetsManager::LoadTexture(texturePath).get();
                        sc.Tint = spriteComponent["Tint"].as<Vector4>();
                    }
                }
                
                auto modelComponent = entity["ModelComponent"];
                if (modelComponent)
                {
                    auto& mc = entt.GetOrAddComponent<ModelComponent>();
                    auto modelPath = modelComponent["Model"].as<std::string>();
                    if (!modelPath.empty()) {
                        mc.ModelData = *AssetsManager::LoadModel(modelPath).get();
                    }
                }
                
                auto rigidBody2DComponent = entity["RigidBody2DComponent"];
                if (rigidBody2DComponent)
                {
                    auto& rb2d = entt.GetOrAddComponent<RigidBody2DComponent>();
                    rb2d.Type = RigidBody2DBodyTypeFromString(rigidBody2DComponent["Type"].as<std::string>());
                    rb2d.FixedRotation = rigidBody2DComponent["FixedRotation"].as<bool>();
                    rb2d.AllowSleep = rigidBody2DComponent["AllowSleep"].as<bool>();
                    rb2d.Awake = rigidBody2DComponent["Awake"].as<bool>();
                    rb2d.GravityScale = rigidBody2DComponent["GravityScale"].as<float>();
                }
                
                auto boxCollider2DComponent = entity["BoxCollider2DComponent"];
                if (boxCollider2DComponent)
                {
                    auto& bc2d = entt.GetOrAddComponent<BoxCollider2DComponent>();
                    bc2d.Density = boxCollider2DComponent["Density"].as<float>();
                    bc2d.Friction = boxCollider2DComponent["Friction"].as<float>();
                    bc2d.Restitution = boxCollider2DComponent["Restitution"].as<float>();
                }
                
                auto cameraComponent = entity["CameraComponent"];
                if (cameraComponent)
                {
                    auto& cc = entt.GetOrAddComponent<CameraComponent>();
                    cc.Active = cameraComponent["Active"].as<bool>();
                    cc.Camera->GetCamera3D().projection = cameraComponent["Projection"].as<int>();
                    cc.Camera->GetCamera3D().fovy = cameraComponent["FOV"].as<float>();
                }
                
                auto luaScriptComponent = entity["LuaScriptComponent"];
                if (luaScriptComponent)
                {
                    auto& lsc = entt.GetOrAddComponent<LuaScriptComponent>();
                    lsc.ScriptPath = luaScriptComponent["Path"].as<std::string>();
                }
                
            }
        }
        return true;
    }
}
