///////////////////////////////////////////////////////////////////////////////
// scenemanager.h
// ============
// manage the preparing and rendering of 3D scenes - textures, materials, lighting
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//  Updated for CS-330-Computational Graphics and Visualization
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ShaderManager.h"
#include "ShapeMeshes.h"

#include <string>
#include <vector>
#include <glm/glm.hpp>

/***********************************************************
 *  SceneManager
 *
 *  This class contains the code for preparing and rendering
 *  3D scenes, including the shader settings.
 ***********************************************************/
class SceneManager
{
public:
    // constructor
    SceneManager(ShaderManager* pShaderManager);
    // destructor
    ~SceneManager();

    struct DirectionalLight {
        glm::vec3 direction;
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        bool bActive;
    };

    struct PointLight {
        glm::vec3 position;

        float constant;
        float linear;
        float quadratic;

        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        bool bActive;
    };

#define TOTAL_POINT_LIGHTS 5


    struct SpotLight {
        glm::vec3 position;
        glm::vec3 direction;
        float cutOff;
        float outerCutOff;

        float constant;
        float linear;
        float quadratic;

        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;

        bool bActive;
    };

    // in SceneManager class
    SpotLight spotLight;
    DirectionalLight directionalLight;
    PointLight pointLights[TOTAL_POINT_LIGHTS];

    struct TEXTURE_INFO
    {
        std::string tag;
        uint32_t ID;
    };

    struct OBJECT_MATERIAL
    {
        glm::vec3 diffuseColor;
        glm::vec3 specularColor;
        float shininess;
        std::string tag;
    };

private:
    ShaderManager* m_pShaderManager;
    ShapeMeshes* m_basicMeshes;
    int m_loadedTextures;
    TEXTURE_INFO m_textureIDs[16];
    std::vector<OBJECT_MATERIAL> m_objectMaterials;

    bool CreateGLTexture(const char* filename, std::string tag);
    void BindGLTextures();
    void DestroyGLTextures();
    int FindTextureID(std::string tag);
    int FindTextureSlot(std::string tag);
    bool FindMaterial(std::string tag, OBJECT_MATERIAL& material);
    void SetupSceneLights();
    void DefineObjectMaterials();

    void SetTransformations(
        glm::vec3 scaleXYZ,
        float XrotationDegrees,
        float YrotationDegrees,
        float ZrotationDegrees,
        glm::vec3 positionXYZ);

    void SetShaderColor(
        float redColorValue,
        float greenColorValue,
        float blueColorValue,
        float alphaValue);

    void SetShaderTexture(std::string textureTag);
    void SetTextureUVScale(float u, float v);
    void SetShaderMaterial(std::string materialTag);

public:
    void PrepareScene();
    void RenderScene();

    // ----------------------------------------------------
    // Scene Shape Management
    // ----------------------------------------------------

    // Enum for types of shapes
    enum class ShapeType
    {
        Box,
        Cylinder,
        Sphere,
        Pyramid,
        Cone,
        Torus
    };

    // Shape class
    class Shape
    {
    public:
        Shape(ShapeType type) : m_type(type) {}

        void SetPosition(float x, float y, float z) { m_position = glm::vec3(x, y, z); }
        void SetScale(float x, float y, float z) { m_scale = glm::vec3(x, y, z); }
        void SetRotation(float x, float y, float z) { m_rotation = glm::vec3(x, y, z); }
        void SetColor(float r, float g, float b) { m_color = glm::vec3(r, g, b); }

        // Step 1: Add getters
        glm::vec3 GetPosition() const { return m_position; }
        glm::vec3 GetScale() const { return m_scale; }
        glm::vec3 GetRotation() const { return m_rotation; }
        glm::vec3 GetColor() const { return m_color; }

        void Render(ShaderManager* shader, ShapeMeshes* meshes)
        {
            if (!shader || !meshes) return;

            shader->setVec4Value("objectColor", glm::vec4(m_color, 1.0f));

            switch (m_type)
            {
            case ShapeType::Box:       meshes->DrawBoxMesh(); break;
            case ShapeType::Cylinder:  meshes->DrawCylinderMesh(true, true, true); break;
            case ShapeType::Sphere:    meshes->DrawSphereMesh(); break;
            case ShapeType::Pyramid:   meshes->DrawPyramid3Mesh(); break;
            case ShapeType::Cone:      meshes->DrawConeMesh(); break;
            case ShapeType::Torus:     meshes->DrawTorusMesh(); break;
            }
        }

    private:
        ShapeType m_type;
        glm::vec3 m_position{ 0.0f };
        glm::vec3 m_scale{ 1.0f };
        glm::vec3 m_rotation{ 0.0f };
        glm::vec3 m_color{ 1.0f };
    };

    // Vector to store shapes in the scene
    std::vector<Shape> m_sceneShapes;

    // Add a shape to the scene
    void AddShapeToScene(const Shape& shape) { m_sceneShapes.push_back(shape); }
};
