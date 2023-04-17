#pragma once

#include <ituGL/application/Application.h>

#include <ituGL/camera/Camera.h>
#include <ituGL/shader/Material.h>
#include <ituGL/geometry/Mesh.h>
#include <ituGL/geometry/Model.h>
#include <ituGL/utils/DearImGui.h>
#include <vector>
#include <glm/mat4x4.hpp>

class Texture2DObject;

class ViewerApplication : public Application
{
public:
    ViewerApplication();

protected:
    void Initialize() override;
    void Update() override;
    void Render() override;
    void Cleanup() override;

private:

    void DrawWater();
    void DrawObject(const Mesh& mesh, Material& material, const glm::mat4& worldMatrix);

    void InitializeModel();
    void InitializeCamera();
    void InitializeLights();
    void CreateTerrainMesh(Mesh& mesh, unsigned int gridX, unsigned int gridY);

    void UpdateCamera();

    void RenderGUI();

private:
    // Helper object for debug GUI
    DearImGui m_imGui;

    // Mouse position for camera controller
    glm::vec2 m_mousePosition;

    // Camera controller parameters
    Camera m_camera;
    glm::vec3 m_cameraPosition;
    float m_cameraTranslationSpeed;
    float m_cameraRotationSpeed;
    bool m_cameraEnabled;
    bool m_cameraEnablePressed;


    // terrain
    Mesh m_terrainPatch;
    std::shared_ptr<Material> m_waterMaterial;

    // Loaded model
    Model m_model;

    // Add light variables
    glm::vec3 m_ambientColor;
    glm::vec3 m_lightColor;
    float m_lightIntensity;
    glm::vec3 m_lightPosition;

    // Specular exponent debug
    float m_specularExponentGrass;
};
