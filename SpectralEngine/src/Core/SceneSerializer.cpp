//
//  SceneSerializer.cpp
//  SpectralEngine
//
//  Created by Nicolas U on 30.04.24.
//

#include "SceneSerializer.hpp"

#include "yaml-cpp/yaml.h"

#include "Objects/Component.hpp"
#include "Renderer/TextureManager.hpp"
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

    static void SerializeObject(YAML::Emitter& out, Object* obj)
    {
        out << YAML::BeginMap; // Object
        
        out << YAML::Key << "Type" << YAML::Value << obj->GetClassName();
        out << YAML::Key << "ID" << YAML::Value << obj->GetUUID();
        out << YAML::Key << "Name" << YAML::Value << obj->GetName();
        
        if (obj->HasComponent(TransformComponent::GetComponentId()))
        {
            out << YAML::Key << "TransformComponent";
            out << YAML::BeginMap; // TransformComponent
            
            TransformComponent& tc = *obj->GetComponent<TransformComponent>();
            out << YAML::Key << "Translation" << YAML::Value << tc.GetTransform().Translation;
            out << YAML::Key << "Rotation" << YAML::Value << tc.GetTransform().Rotation;
            out << YAML::Key << "Scale" << YAML::Value << tc.GetTransform().Scale;
            
            out << YAML::EndMap; // TransformComponent
        }
        
        if (obj->HasComponent(SpriteComponent::GetComponentId()))
        {
            out << YAML::Key << "SpriteComponent";
            out << YAML::BeginMap; // SpriteComponent
            
            SpriteComponent& sc = *obj->GetComponent<SpriteComponent>();
            out << YAML::Key << "Texture" << YAML::Value << TextureManager::GetTexturePath(sc.GetTexture());
            out << YAML::Key << "Tint" << YAML::Value << sc.GetTint();
            
            out << YAML::EndMap; // SpriteComponent
        }
        
        if (obj->HasComponent(CameraComponent::GetComponentId()))
        {
            out << YAML::Key << "CameraComponent";
            out << YAML::BeginMap; // CameraComponent
            
            CameraComponent& cc = *obj->GetComponent<CameraComponent>();
            out << YAML::Key << "Active" << YAML::Value << cc.IsActive();
            
            out << YAML::EndMap; // CameraComponent
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
        out << YAML::Key << "Objects" << YAML::Value << YAML::BeginSeq;
        
        for (const auto& [_, v] : m_Scene->m_ObjectRegistry)
        {
            if (!v) {
                continue;
            }
            SerializeObject(out, v.get());
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
        
        auto objects = data["Objects"];
        if (objects)
        {
            for (auto object : objects)
            {
                Object* obj = nullptr;
                
                uint64_t uuid = object["ID"].as<Spectral::UUID>();
                
                auto type = object["Type"];
                if (type) {
                    const std::string& objectType = type.as<std::string>();
                    if (objectType == "Object") {
                        obj = m_Scene->CreateObject<Object>(uuid);
                    } else if (objectType == "StaticObject") {
                        obj = m_Scene->CreateObject<StaticObject>(uuid);
                    } else if (objectType == "RuntimeObject") {
                        obj = m_Scene->CreateObject<RuntimeObject>(uuid);
                    }
                }
            
                if (!obj) {
                    // print some error
                    SP_LOG_ERORR("SceneSerializer::Deserialize() - Object is NULL, invalid Type");
                    return false;
                }
                
                const std::string& name = object["Name"].as<std::string>();
                obj->SetName(name);
                
                SP_LOG_INFO("Deserializing object : {0}, {1}", uuid, name);
                
                auto transformComponent = object["TransformComponent"];
                if (transformComponent)
                {
                    TransformComponent& tc = *obj->GetOrAddComponent<TransformComponent>();
                    tc.GetTransform().Translation = transformComponent["Translation"].as<Vector3>();
                    tc.GetTransform().Rotation = transformComponent["Rotation"].as<Vector3>();
                    tc.GetTransform().Scale = transformComponent["Scale"].as<Vector3>();
                }
                
                auto spriteComponent = object["SpriteComponent"];
                if (spriteComponent)
                {
                    SpriteComponent& sc = *obj->GetOrAddComponent<SpriteComponent>();
                    sc.SetTexture(TextureManager::LoadTexture(spriteComponent["Texture"].as<std::string>()).get());
                    sc.GetTint() = spriteComponent["Tint"].as<Vector4>();
                }
                
                auto cameraComponent = object["CameraComponent"];
                if (cameraComponent) 
                {
                    CameraComponent& cc = *obj->GetOrAddComponent<CameraComponent>();
                    cc.IsActive() = cameraComponent["Active"].as<bool>();
                }
                
            }
        }
        return true;
    }
}
