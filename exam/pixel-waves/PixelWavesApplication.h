#pragma once

#include <ituGL/application/Application.h>

#include <ituGL/scene/Scene.h>
#include <ituGL/texture/FramebufferObject.h>
#include <ituGL/renderer/Renderer.h>
#include <ituGL/camera/CameraController.h>
#include <ituGL/utils/DearImGui.h>
#include <ituGL/geometry/Mesh.h>
#include <array>

class Texture2DObject;
class TextureCubemapObject;
class Material;

class PixelWavesApplication : public Application
{
public:
    PixelWavesApplication();

protected:
    void Initialize() override;
    void Update() override;
    void Render() override;
    void Cleanup() override;

private:
    void InitializeCamera();
    void InitializeLights();
    void InitializeTextures();
    void InitializeMaterials();
    void InitializeModels();
    void InitializeFramebuffers();
    void InitializeRenderer();
    void InitializeWaterMesh();

    const float GetWaterHeight() { return waterHeight; };
    const glm::mat4 GetReflectWorldMatrix() { return m_worldReflectMatrix; };

    std::shared_ptr<Material> CreatePostFXMaterial(const char* fragmentShaderPath, std::shared_ptr<Texture2DObject> sourceTexture = nullptr);
    std::shared_ptr<Material> CreateCombineMaterial(const char* fragmentShaderPath, std::shared_ptr<Texture2DObject> sourceTexture = nullptr);

    Renderer::UpdateTransformsFunction GetFullscreenTransformFunction(std::shared_ptr<ShaderProgram> shaderProgramPtr) const;

    void RenderGUI();

private:
    // Helper object for debug GUI
    DearImGui m_imGui;

    // Camera controller
    CameraController m_cameraController;


    // Camera settings
    std::shared_ptr<Camera> m_camera;
    std::shared_ptr<Camera> m_reflectionCamera;
    float nearPlane;
    float farPlane;

    glm::mat4 m_worldReflectMatrix;
    bool m_renderFlip;

    // Global scene
    Scene m_scene;

    // Renderer
    Renderer m_renderer;

    // Skybox texture
    std::shared_ptr<TextureCubemapObject> m_skyboxTexture;

    // Water
    std::shared_ptr<Model> m_waterModel;
    std::shared_ptr<Mesh> m_waterMesh;
    std::shared_ptr<Texture2DObject> m_waterTexture;
    float waterHeight = 0.0f;

    // Materials
    std::shared_ptr<Material> m_waterMaterial;
    std::shared_ptr<Material> m_defaultMaterial;
    std::shared_ptr<Material> m_deferredMaterial;
    std::shared_ptr<Material> m_invDeferredMaterial;
    std::shared_ptr<Material> m_composeMaterial;

    // Framebuffers
    std::shared_ptr<FramebufferObject> m_sceneFramebuffer;
    std::shared_ptr<FramebufferObject> m_reflectSceneFramebuffer;

    std::shared_ptr<Texture2DObject> m_sceneTexture;
    std::shared_ptr<Texture2DObject> m_reflectSceneTexture;

    std::shared_ptr<Texture2DObject> m_depthTexture;
    std::shared_ptr<Texture2DObject> m_depthTextureCopy;
    std::shared_ptr<Texture2DObject> m_reflectDepthTexture;

    std::shared_ptr<FramebufferObject> m_waterFramebuffer;
    std::shared_ptr<Texture2DObject> m_waterDepthTexture;

    std::array<std::shared_ptr<FramebufferObject>, 2> m_tempFramebuffers;
    std::array<std::shared_ptr<Texture2DObject>, 2> m_tempTextures;

    // Water configuration values
    float m_distortionFrequency;
    float m_distortionStrength;
    float m_distortionSpeed;

    // Post-procesing configuration values
    float m_exposure;
    float m_contrast;
    float m_hueShift;
    float m_pixelation;
    int m_downsampling;
    float m_saturation;
    float m_sharpness;
    glm::vec3 m_colorFilter;

    // Camera settings
    bool placedModel = false;
};
