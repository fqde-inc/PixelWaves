#include "PixelWavesApplication.h"

#include <ituGL/asset/TextureCubemapLoader.h>
#include <ituGL/asset/ShaderLoader.h>
#include <ituGL/asset/ModelLoader.h>

#include <ituGL/camera/Camera.h>
#include <ituGL/scene/SceneCamera.h>
#include <ituGL/scene/Transform.h>

#include <ituGL/lighting/DirectionalLight.h>
#include <ituGL/lighting/PointLight.h>
#include <ituGL/scene/SceneLight.h>

#include <ituGL/asset/Texture2DLoader.h>
#include <ituGL/shader/ShaderUniformCollection.h>
#include <ituGL/shader/Material.h>
#include <ituGL/geometry/Model.h>
#include <ituGL/scene/SceneModel.h>
#include <ituGL/geometry/Mesh.h>

#include <ituGL/renderer/SkyboxRenderPass.h>
#include <ituGL/renderer/WaterRenderPass.h>
#include <ituGL/renderer/GBufferRenderPass.h>
#include <ituGL/renderer/DeferredRenderPass.h>
#include <ituGL/renderer/PostFXRenderPass.h>
#include <ituGL/scene/RendererSceneVisitor.h>

#include <ituGL/geometry/VertexFormat.h>
#include <ituGL/texture/Texture2DObject.h>

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

#include <ituGL/scene/ImGuiSceneVisitor.h>
#include <imgui.h>

PixelWavesApplication::PixelWavesApplication()
    : Application(1024, 1024, "Pixel Waves")
    , m_renderer(GetDevice())
    , m_sceneFramebuffer(std::make_shared<FramebufferObject>())
    , m_reflectSceneFramebuffer(std::make_shared<FramebufferObject>())
    , m_exposure(1.0f)
    , m_contrast(1.06f)
    , m_hueShift(-0.07f)
    , m_sharpness(0.5f)
    , m_saturation(1.4f)
    , m_colorFilter(1.0f)
    , m_pixelation(256.0f)
    , m_downsampling(4)
    , m_distortionSpeed(2.5f)
    , m_distortionStrength(0.0035f)
    , m_distortionFrequency(0.1f)
    , nearPlane(0.01f)
    , farPlane(100.0f)
{
}

void PixelWavesApplication::Initialize()
{
    Application::Initialize();

    // Initialize DearImGUI
    m_imGui.Initialize(GetMainWindow());

    InitializeCamera();
    InitializeLights();
    InitializeTextures();
    InitializeMaterials();
    InitializeWaterMesh();
    InitializeModels();
    InitializeRenderer();

    GetDevice().EnableFeature(GL_DEPTH_TEST);
    //GetDevice().SetWireframeEnabled(true);
}

void PixelWavesApplication::Update()
{
    Application::Update();

    // WaterHeight
    const float pi = 3.1416f;
    waterHeight = 0.05f * sin(2 * pi * GetCurrentTime() / 15.0f);

    // Update camera controller
    m_cameraController.Update(GetMainWindow(), GetDeltaTime());
    
    glm::vec3 right, up, forward;
    m_camera->ExtractVectors(right, up, forward);
    glm::vec3 translation = glm::vec3(m_camera->ExtractTranslation());

    // Move camera from under the water height
    translation.y = - ( translation.y - waterHeight );
    forward = glm::reflect(forward, glm::vec3(0, 1 ,0) );

    // Update reflection camera
    m_reflectionCamera->SetViewMatrix(glm::lookAt(translation, forward, up) );

    // Add the scene nodes to the renderer
    RendererSceneVisitor rendererSceneVisitor(m_renderer);
    m_scene.AcceptVisitor(rendererSceneVisitor);

    if (!placedModel) {
        auto transform = m_scene.GetSceneNode("House")->GetTransform();
        transform->SetTranslation(glm::vec3(0.0f, 0.45f, 0.0f));
        placedModel = true;
    }

    {
        auto gullTransform = m_scene.GetSceneNode("Gull")->GetTransform();
        glm::vec3 position = gullTransform->GetTranslation();

        float radius = 2.0f;
        float birdAngle = m_currentTime * 0.7f;

        position = glm::vec3(radius * sin(birdAngle), 1.8f + sin(birdAngle / 4.0f) / 2.0f, radius * cos(birdAngle));
        gullTransform->SetTranslation(position);

        glm::vec3 euler(0.0f, birdAngle + glm::radians(90.0f), -0.4f);
        gullTransform->SetRotation(euler);
    }

    {
        auto gullTransform = m_scene.GetSceneNode("Gull2")->GetTransform();
        glm::vec3 position = gullTransform->GetTranslation();

        float radius = 1.9f;
        float birdAngle = -m_currentTime * 0.75f;

        position = glm::vec3(radius * sin(birdAngle), 1.8f + sin(birdAngle / 4.0f) / 1.5f, radius * cos(birdAngle));
        gullTransform->SetTranslation(position);

        glm::vec3 euler(0.0f, birdAngle - glm::radians(90.0f), 0.4f);
        gullTransform->SetRotation(euler);
    }
}

void PixelWavesApplication::Render()
{
    Application::Render();

    GetDevice().Clear(true, Color(0.0f, 0.0f, 0.0f, 1.0f), true, 1.0f);

    // Render the scene
    m_renderer.Render();

    // Render the debug user interface
    RenderGUI();
}

void PixelWavesApplication::Cleanup()
{
    // Cleanup DearImGUI
    m_imGui.Cleanup();

    Application::Cleanup();
}

void PixelWavesApplication::InitializeCamera()
{
    m_camera = std::make_shared<Camera>();
    m_reflectionCamera = std::make_shared<Camera>();

    m_camera->SetViewMatrix(glm::vec3(5, 1.5, -3.5), glm::vec3(0.5f, 0.0f, 0), glm::vec3(0, 1, 0));
    m_camera->SetPerspectiveProjectionMatrix(45.0f, 1.0f, nearPlane, farPlane);

    m_reflectionCamera->SetViewMatrix(glm::vec3(5, -1.5, -3.5), glm::vec3(0.5f, 0.0f, 0), glm::vec3(0, 1, 0));
    m_reflectionCamera->SetPerspectiveProjectionMatrix(90.0f, 1.0f, nearPlane, farPlane);

    // Create a scene node for the camera
    std::shared_ptr<SceneCamera> sceneCamera = std::make_shared<SceneCamera>("camera", m_camera);

    // Add the camera node to the scene
    m_scene.AddSceneNode(sceneCamera);

    // Set the main camera scene node to be controlled by the camera controller
    m_cameraController.SetCamera(sceneCamera);
}

void PixelWavesApplication::InitializeLights()
{
    // Create a directional light and add it to the scene
    std::shared_ptr<DirectionalLight> directionalLight = std::make_shared<DirectionalLight>();
    directionalLight->SetDirection(glm::vec3(0.0f, -1.0f, -0.3f)); // It will be normalized inside the function
    directionalLight->SetIntensity(3.0f);
    m_scene.AddSceneNode(std::make_shared<SceneLight>("directional light", directionalLight));

    // Create a point light and add it to the scene
    //std::shared_ptr<PointLight> pointLight = std::make_shared<PointLight>();
    //pointLight->SetPosition(glm::vec3(0, 0, 0));
    //pointLight->SetDistanceAttenuation(glm::vec2(5.0f, 10.0f));
    //m_scene.AddSceneNode(std::make_shared<SceneLight>("point light", pointLight));
}

void PixelWavesApplication::InitializeTextures()
{

}

void PixelWavesApplication::InitializeMaterials()
{
    // G-buffer material
    {
        // Load and build shader
        std::vector<const char*> vertexShaderPaths;
        vertexShaderPaths.push_back("shaders/version330.glsl");
        vertexShaderPaths.push_back("shaders/default.vert");
        Shader vertexShader = ShaderLoader(Shader::VertexShader).Load(vertexShaderPaths);

        std::vector<const char*> fragmentShaderPaths;
        fragmentShaderPaths.push_back("shaders/version330.glsl");
        fragmentShaderPaths.push_back("shaders/utils.glsl");
        fragmentShaderPaths.push_back("shaders/default.frag");
        Shader fragmentShader = ShaderLoader(Shader::FragmentShader).Load(fragmentShaderPaths);

        std::shared_ptr<ShaderProgram> shaderProgramPtr = std::make_shared<ShaderProgram>();
        shaderProgramPtr->Build(vertexShader, fragmentShader);

        // Get transform related uniform locations
        ShaderProgram::Location waterPlaneLocation = shaderProgramPtr->GetUniformLocation("WaterPlane");

        // Get transform related uniform locations
        ShaderProgram::Location worldMatrixLocation = shaderProgramPtr->GetUniformLocation("WorldMatrix");
        ShaderProgram::Location worldViewMatrixLocation = shaderProgramPtr->GetUniformLocation("WorldViewMatrix");
        ShaderProgram::Location worldViewProjMatrixLocation = shaderProgramPtr->GetUniformLocation("WorldViewProjMatrix");

        // Register shader with renderer
        // Register shader with renderer
        m_renderer.RegisterShaderProgram(shaderProgramPtr,
            [=](const ShaderProgram& shaderProgram, const glm::mat4& worldMatrix, const Camera& camera, bool cameraChanged)
            {
                shaderProgram.SetUniform(waterPlaneLocation, glm::vec4( 0, 1.0f, 0, - GetWaterHeight()) );
                shaderProgram.SetUniform(worldMatrixLocation, worldMatrix);
                shaderProgram.SetUniform(worldViewMatrixLocation, camera.GetViewMatrix() * worldMatrix);
                shaderProgram.SetUniform(worldViewProjMatrixLocation, camera.GetViewProjectionMatrix() * worldMatrix);
            },
            nullptr
        );

        // Filter out uniforms that are not material properties
        ShaderUniformCollection::NameSet filteredUniforms;
        filteredUniforms.insert("WorldMatrix");
        filteredUniforms.insert("WorldViewMatrix");
        filteredUniforms.insert("WorldViewProjMatrix");

        // Create material
        m_defaultMaterial = std::make_shared<Material>(shaderProgramPtr, filteredUniforms);
        m_defaultMaterial->SetUniformValue("Color", glm::vec3(1.0f));
    }

    // Water material
    {
        // Water texture
        m_waterTexture = Texture2DLoader::LoadTextureShared("textures/water.png", TextureObject::FormatRGBA, TextureObject::InternalFormatRGBA);
        m_waterTexture->Bind();
        m_waterTexture->SetParameter(TextureObject::ParameterEnum::WrapS, GL_CLAMP_TO_EDGE);
        m_waterTexture->SetParameter(TextureObject::ParameterEnum::WrapT, GL_CLAMP_TO_EDGE);
        m_waterTexture->SetParameter(TextureObject::ParameterEnum::MinFilter, GL_NEAREST);
        m_waterTexture->SetParameter(TextureObject::ParameterEnum::MagFilter, GL_NEAREST);
        m_waterTexture->GenerateMipmap();
        Texture2DObject::Unbind();

        // Load and build shader
        std::vector<const char*> vertexShaderPaths;
        vertexShaderPaths.push_back("shaders/version330.glsl");
        vertexShaderPaths.push_back("shaders/water.vert");
        Shader vertexShader = ShaderLoader(Shader::VertexShader).Load(vertexShaderPaths);

        std::vector<const char*> fragmentShaderPaths;
        fragmentShaderPaths.push_back("shaders/version330.glsl");
        fragmentShaderPaths.push_back("shaders/utils.glsl");
        fragmentShaderPaths.push_back("shaders/water.frag");
        Shader fragmentShader = ShaderLoader(Shader::FragmentShader).Load(fragmentShaderPaths);

        std::shared_ptr<ShaderProgram> shaderProgramPtr = std::make_shared<ShaderProgram>();
        shaderProgramPtr->Build(vertexShader, fragmentShader);

        // Get transform related uniform locations
        ShaderProgram::Location heightLocation = shaderProgramPtr->GetUniformLocation("Height");
        ShaderProgram::Location timeLocation = shaderProgramPtr->GetUniformLocation("Time");
        ShaderProgram::Location mirrorViewLocation = shaderProgramPtr->GetUniformLocation("MirrorViewMatrix");
        ShaderProgram::Location viewProjLocation = shaderProgramPtr->GetUniformLocation("ViewProjMatrix");
        ShaderProgram::Location worldMatrixLocation = shaderProgramPtr->GetUniformLocation("WorldMatrix");
        ShaderProgram::Location worldViewMatrixLocation = shaderProgramPtr->GetUniformLocation("WorldViewMatrix");
        ShaderProgram::Location worldViewProjMatrixLocation = shaderProgramPtr->GetUniformLocation("WorldViewProjMatrix");
        
        // Register shader with renderer
        m_renderer.RegisterShaderProgram(shaderProgramPtr,
            [=](const ShaderProgram& shaderProgram, const glm::mat4& worldMatrix, const Camera& camera, bool cameraChanged)
            {
                shaderProgram.SetUniform(heightLocation, GetWaterHeight());
                shaderProgram.SetUniform(worldMatrixLocation, worldMatrix);

                shaderProgram.SetUniform(mirrorViewLocation, m_reflectionCamera->GetViewProjectionMatrix() );
                
                shaderProgram.SetUniform(viewProjLocation, camera.GetProjectionMatrix() * camera.GetViewMatrix() );
                shaderProgram.SetUniform(worldViewMatrixLocation, camera.GetViewMatrix() * worldMatrix);
                shaderProgram.SetUniform(worldViewProjMatrixLocation, camera.GetViewProjectionMatrix() * worldMatrix);
                shaderProgram.SetUniform(timeLocation, GetCurrentTime());
            },
            m_renderer.GetDefaultUpdateLightsFunction(*shaderProgramPtr)
        );

        // Filter out uniforms that are not material properties
        ShaderUniformCollection::NameSet filteredUniforms;
        filteredUniforms.insert("WorldMatrix");
        filteredUniforms.insert("WorldViewMatrix");
        filteredUniforms.insert("WorldViewProjMatrix");

        // Create material
        m_waterMaterial = std::make_shared<Material>(shaderProgramPtr, filteredUniforms);

        m_waterMaterial->SetUniformValue("Color", glm::vec4(0.137, 0.537, 0.855, 0.7f) );

        m_waterMaterial->SetUniformValue("Height", 1.5f);
        m_waterMaterial->SetUniformValue("Speed", 1.5f);
        m_waterMaterial->SetUniformValue("Amplitude", 0.01f);
        m_waterMaterial->SetUniformValue("Wavelength", 10.0f);
        m_waterMaterial->SetUniformValue("Time", GetCurrentTime());

        m_waterMaterial->SetUniformValue("DistortionSpeed", m_distortionSpeed);
        m_waterMaterial->SetUniformValue("DistortionStrength", m_distortionStrength);
        m_waterMaterial->SetUniformValue("DistortionFrequency", m_distortionFrequency);

        m_waterMaterial->SetUniformValue("ColorTextureScale", glm::vec2(1.0f));

        m_waterMaterial->SetBlendEquation(Material::BlendEquation::Add);
        m_waterMaterial->SetBlendParams(Material::BlendParam::SourceAlpha, Material::BlendParam::OneMinusSourceAlpha);
    }

    // Deferred material
    {
        std::vector<const char*> vertexShaderPaths;
        vertexShaderPaths.push_back("shaders/version330.glsl");
        vertexShaderPaths.push_back("shaders/renderer/deferred.vert");
        Shader vertexShader = ShaderLoader(Shader::VertexShader).Load(vertexShaderPaths);

        std::vector<const char*> fragmentShaderPaths;
        fragmentShaderPaths.push_back("shaders/version330.glsl");
        fragmentShaderPaths.push_back("shaders/utils.glsl");
        fragmentShaderPaths.push_back("shaders/lambert-ggx.glsl");
        fragmentShaderPaths.push_back("shaders/lighting.glsl");
        fragmentShaderPaths.push_back("shaders/renderer/deferred.frag");
        Shader fragmentShader = ShaderLoader(Shader::FragmentShader).Load(fragmentShaderPaths);

        std::shared_ptr<ShaderProgram> shaderProgramPtr = std::make_shared<ShaderProgram>();
        std::shared_ptr<ShaderProgram> reflectShaderProgramPtr = std::make_shared<ShaderProgram>();
        shaderProgramPtr->Build(vertexShader, fragmentShader);

        // Filter out uniforms that are not material properties
        ShaderUniformCollection::NameSet filteredUniforms;
        filteredUniforms.insert("InvViewMatrix");
        filteredUniforms.insert("InvProjMatrix");
        filteredUniforms.insert("WorldViewProjMatrix");
        filteredUniforms.insert("LightIndirect");
        filteredUniforms.insert("LightColor");
        filteredUniforms.insert("LightPosition");
        filteredUniforms.insert("LightDirection");
        filteredUniforms.insert("LightAttenuation");

        // Get transform related uniform locations
        ShaderProgram::Location invViewMatrixLocation = shaderProgramPtr->GetUniformLocation("InvViewMatrix");
        ShaderProgram::Location invProjMatrixLocation = shaderProgramPtr->GetUniformLocation("InvProjMatrix");
        ShaderProgram::Location worldViewProjMatrixLocation = shaderProgramPtr->GetUniformLocation("WorldViewProjMatrix");

        // Register shader with renderer
        m_renderer.RegisterShaderProgram(shaderProgramPtr,
            [=](const ShaderProgram& shaderProgram, const glm::mat4& worldMatrix, const Camera& camera, bool cameraChanged)
            {
               if (cameraChanged)
               {
                    shaderProgram.SetUniform(invViewMatrixLocation, glm::inverse(camera.GetViewMatrix()));
                    shaderProgram.SetUniform(invProjMatrixLocation, glm::inverse(camera.GetProjectionMatrix()));
               }
                shaderProgram.SetUniform(worldViewProjMatrixLocation, camera.GetViewProjectionMatrix() * worldMatrix);
            },
            m_renderer.GetDefaultUpdateLightsFunction(*shaderProgramPtr)
        );

        // Create materials
        m_deferredMaterial = std::make_shared<Material>(shaderProgramPtr, filteredUniforms);
    }


    // Reflection material
    {
        std::vector<const char*> vertexShaderPaths;
        vertexShaderPaths.push_back("shaders/version330.glsl");
        vertexShaderPaths.push_back("shaders/renderer/deferred.vert");
        Shader vertexShader = ShaderLoader(Shader::VertexShader).Load(vertexShaderPaths);

        std::vector<const char*> fragmentShaderPaths;
        fragmentShaderPaths.push_back("shaders/version330.glsl");
        fragmentShaderPaths.push_back("shaders/utils.glsl");
        fragmentShaderPaths.push_back("shaders/lambert-ggx.glsl");
        fragmentShaderPaths.push_back("shaders/lighting.glsl");
        fragmentShaderPaths.push_back("shaders/renderer/deferred.frag");
        Shader fragmentShader = ShaderLoader(Shader::FragmentShader).Load(fragmentShaderPaths);

        std::shared_ptr<ShaderProgram> shaderProgramPtr = std::make_shared<ShaderProgram>();
        std::shared_ptr<ShaderProgram> reflectShaderProgramPtr = std::make_shared<ShaderProgram>();
        shaderProgramPtr->Build(vertexShader, fragmentShader);

        // Filter out uniforms that are not material properties
        ShaderUniformCollection::NameSet filteredUniforms;
        filteredUniforms.insert("InvViewMatrix");
        filteredUniforms.insert("InvProjMatrix");
        filteredUniforms.insert("WorldViewProjMatrix");
        filteredUniforms.insert("LightIndirect");
        filteredUniforms.insert("LightColor");
        filteredUniforms.insert("LightPosition");
        filteredUniforms.insert("LightDirection");
        filteredUniforms.insert("LightAttenuation");

        // Get transform related uniform locations
        ShaderProgram::Location invViewMatrixLocation = shaderProgramPtr->GetUniformLocation("InvViewMatrix");
        ShaderProgram::Location invProjMatrixLocation = shaderProgramPtr->GetUniformLocation("InvProjMatrix");
        ShaderProgram::Location worldViewProjMatrixLocation = shaderProgramPtr->GetUniformLocation("WorldViewProjMatrix");

        // Register shader with renderer
        m_renderer.RegisterShaderProgram(shaderProgramPtr,
            [=](const ShaderProgram& shaderProgram, const glm::mat4& worldMatrix, const Camera& camera, bool cameraChanged)
            {
                m_worldReflectMatrix = worldMatrix;

                if (cameraChanged)
                {
                shaderProgram.SetUniform(invViewMatrixLocation, glm::inverse(camera.GetViewMatrix()));
                shaderProgram.SetUniform(invProjMatrixLocation, glm::inverse(camera.GetProjectionMatrix()));
                }
                shaderProgram.SetUniform(worldViewProjMatrixLocation, camera.GetViewProjectionMatrix() * worldMatrix);
            },
            m_renderer.GetDefaultUpdateLightsFunction(*shaderProgramPtr)
        );

        // Create materials
        m_invDeferredMaterial = std::make_shared<Material>(shaderProgramPtr, filteredUniforms);
    }

}

void PixelWavesApplication::InitializeModels()
{
    m_skyboxTexture = TextureCubemapLoader::LoadTextureShared("models/skybox/animeee.hdr", TextureObject::FormatRGB, TextureObject::InternalFormatRGB16F);

    m_skyboxTexture->Bind();
    float maxLod;
    m_skyboxTexture->GetParameter(TextureObject::ParameterFloat::MaxLod, maxLod);
    TextureCubemapObject::Unbind();

    // Set the environment texture on the deferred material
    m_deferredMaterial->SetUniformValue("EnvironmentTexture", m_skyboxTexture);
    m_deferredMaterial->SetUniformValue("EnvironmentMaxLod", maxLod);

    // Configure loader
    ModelLoader loader(m_defaultMaterial);

    // Create a new material copy for each submaterial
    loader.SetCreateMaterials(true);

    // Flip vertically textures loaded by the model loader
    loader.GetTexture2DLoader().SetFlipVertical(true);

    // Link vertex properties to attributes
    loader.SetMaterialAttribute(VertexAttribute::Semantic::Position, "VertexPosition");
    loader.SetMaterialAttribute(VertexAttribute::Semantic::Normal, "VertexNormal");
    loader.SetMaterialAttribute(VertexAttribute::Semantic::Tangent, "VertexTangent");
    loader.SetMaterialAttribute(VertexAttribute::Semantic::Bitangent, "VertexBitangent");
    loader.SetMaterialAttribute(VertexAttribute::Semantic::TexCoord0, "VertexTexCoord");

    // Link material properties to uniforms
    loader.SetMaterialProperty(ModelLoader::MaterialProperty::DiffuseColor, "Color");
    loader.SetMaterialProperty(ModelLoader::MaterialProperty::DiffuseTexture, "ColorTexture");
    loader.SetMaterialProperty(ModelLoader::MaterialProperty::NormalTexture, "NormalTexture");
    loader.SetMaterialProperty(ModelLoader::MaterialProperty::SpecularTexture, "SpecularTexture");

    // Load models
    std::shared_ptr<Model> houseModel = loader.LoadShared("models/house/House.obj");
    m_scene.AddSceneNode(std::make_shared<SceneModel>("House", houseModel));

    std::shared_ptr<Model> gullModel = loader.LoadShared("models/gull/gull.obj");
    m_scene.AddSceneNode(std::make_shared<SceneModel>("Gull", gullModel));

    std::shared_ptr<Model> gullModel2 = loader.LoadShared("models/gull/gull.obj");
    m_scene.AddSceneNode(std::make_shared<SceneModel>("Gull2", gullModel2));

    //m_scene.AddSceneNode(std::make_shared<SceneModel>("water", m_waterModel));
    auto gullTransform = m_scene.GetSceneNode("Gull")->GetTransform();
    gullTransform->SetScale(glm::vec3(.01f));

    auto gullTransform2 = m_scene.GetSceneNode("Gull2")->GetTransform();
    gullTransform2->SetScale(glm::vec3(.008f));
    
    // Water
    //m_scene.AddSceneNode(std::make_shared<SceneModel>("water", m_waterModel));
    //auto waterTransform = m_scene.GetSceneNode("water")->GetTransform();
    //waterTransform->SetTranslation(glm::vec3(-2.0f, 0, -2.0f));
}

void PixelWavesApplication::InitializeWaterMesh()
{
    m_waterMesh = std::make_shared<Mesh>();
    Mesh& mesh = *m_waterMesh.get();

    // Define the vertex structure
    struct Vertex
    {
        Vertex() = default;
        Vertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec2 texCoord)
            : position(position), normal(normal), texCoord(texCoord) {}
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;
    };

    // Define the vertex format (should match the vertex structure)
    VertexFormat vertexFormat;
    vertexFormat.AddVertexAttribute<float>(3);
    vertexFormat.AddVertexAttribute<float>(3);
    vertexFormat.AddVertexAttribute<float>(2);

    // List of vertices (VBO)
    std::vector<Vertex> vertices;

    // List of indices (EBO)
    std::vector<unsigned int> indices;

    // Grid scale to convert the entire grid to size 1x1
    int size = 16;
    glm::vec2 pos = glm::vec2(size/2.0f);
    glm::vec2 scale(1.0f / (size - 1), 1.0f / (size - 1));

    // Number of columns and rows
    unsigned int columnCount = size;
    unsigned int rowCount = size;

    // Iterate over each VERTEX
    for (unsigned int j = 0; j < rowCount; ++j)
    {
        for (unsigned int i = 0; i < columnCount; ++i)
        {
            // Vertex data for this vertex only
            glm::vec3 position(i - pos.x, 0.0f, j - pos.y);
            glm::vec3 normal(0.0f, 1.0f, 0.0f);
            glm::vec2 texCoord(i - pos.x, j - pos.y);
            vertices.emplace_back(position, normal, texCoord);

            // Index data for quad formed by previous vertices and current
            if (i > 0 && j > 0)
            {
                unsigned int top_right = j * columnCount + i; // Current vertex
                unsigned int top_left = top_right - 1;
                unsigned int bottom_right = top_right - columnCount;
                unsigned int bottom_left = bottom_right - 1;

                //Triangle 1
                indices.push_back(bottom_right);
                indices.push_back(bottom_left);
                indices.push_back(top_left);

                //Triangle 2
                indices.push_back(bottom_right);
                indices.push_back(top_left);
                indices.push_back(top_right);
            }
        }
    }
    
    mesh.AddSubmesh<Vertex, unsigned int, VertexFormat::LayoutIterator> (
        Drawcall::Primitive::Triangles, 
        vertices, 
        indices,
        vertexFormat.LayoutBegin( static_cast<int>(vertices.size() ), 
        true /* interleaved */
    ), vertexFormat.LayoutEnd());
}

void PixelWavesApplication::InitializeFramebuffers()
{
    int width, height;
    GetMainWindow().GetDimensions(width, height);

    // Scene Texture
    m_sceneTexture = std::make_shared<Texture2DObject>();
    m_sceneTexture->Bind();
    m_sceneTexture->SetImage(0, width, height, TextureObject::FormatRGBA, TextureObject::InternalFormat::InternalFormatRGBA16F);
    m_sceneTexture->SetParameter(TextureObject::ParameterEnum::WrapS, GL_CLAMP_TO_EDGE);
    m_sceneTexture->SetParameter(TextureObject::ParameterEnum::WrapT, GL_CLAMP_TO_EDGE);
    m_sceneTexture->SetParameter(TextureObject::ParameterEnum::MinFilter, GL_NEAREST);
    m_sceneTexture->SetParameter(TextureObject::ParameterEnum::MagFilter, GL_NEAREST);
    Texture2DObject::Unbind();

    // Scene framebuffer
    m_sceneFramebuffer->Bind();
    m_sceneFramebuffer->SetTexture(FramebufferObject::Target::Draw, FramebufferObject::Attachment::Depth, *m_depthTexture);
    m_sceneFramebuffer->SetTexture(FramebufferObject::Target::Draw, FramebufferObject::Attachment::Color0, *m_sceneTexture);
    m_sceneFramebuffer->SetDrawBuffers(std::array<FramebufferObject::Attachment, 1>({ FramebufferObject::Attachment::Color0 }));
    FramebufferObject::Unbind();
    
    // Water depth
    m_waterDepthTexture = std::make_shared<Texture2DObject>();
    m_waterDepthTexture->Bind();
    m_waterDepthTexture->SetImage(0, width, height, TextureObject::FormatDepth, TextureObject::InternalFormatDepth);
    m_waterDepthTexture->SetParameter(TextureObject::ParameterEnum::WrapS, GL_CLAMP_TO_EDGE);
    m_waterDepthTexture->SetParameter(TextureObject::ParameterEnum::WrapT, GL_CLAMP_TO_EDGE);
    m_waterDepthTexture->SetParameter(TextureObject::ParameterEnum::MinFilter, GL_NEAREST);
    m_waterDepthTexture->SetParameter(TextureObject::ParameterEnum::MagFilter, GL_NEAREST);
    Texture2DObject::Unbind();

    // Water framebuffer
    m_waterFramebuffer = std::make_shared<FramebufferObject>();
    m_waterFramebuffer->Bind();
    //m_waterFramebuffer->SetTexture(FramebufferObject::Target::Draw, FramebufferObject::Attachment::Depth, *m_waterDepthTexture);
    //m_waterFramebuffer->SetTexture(FramebufferObject::Target::Draw, FramebufferObject::Attachment::Color0, *m_waterTexture);
    //m_waterFramebuffer->SetDrawBuffers(std::array<FramebufferObject::Attachment, 1>({ FramebufferObject::Attachment::Color0 }));
    FramebufferObject::Unbind();

    // Add temp textures and frame buffers
    for (int i = 0; i < m_tempFramebuffers.size(); ++i)
    {
        m_tempTextures[i] = std::make_shared<Texture2DObject>();
        m_tempTextures[i]->Bind();
        m_tempTextures[i]->SetImage(0, width, height, TextureObject::FormatRGBA, TextureObject::InternalFormat::InternalFormatRGBA16F);
        m_tempTextures[i]->SetParameter(TextureObject::ParameterEnum::WrapS, GL_CLAMP_TO_EDGE);
        m_tempTextures[i]->SetParameter(TextureObject::ParameterEnum::WrapT, GL_CLAMP_TO_EDGE);
        m_tempTextures[i]->SetParameter(TextureObject::ParameterEnum::MinFilter, GL_NEAREST);
        m_tempTextures[i]->SetParameter(TextureObject::ParameterEnum::MagFilter, GL_NEAREST);
        
        m_tempFramebuffers[i] = std::make_shared<FramebufferObject>();
        m_tempFramebuffers[i]->Bind();
        m_tempFramebuffers[i]->SetTexture(FramebufferObject::Target::Draw, FramebufferObject::Attachment::Color0, *m_tempTextures[i]);
        m_tempFramebuffers[i]->SetDrawBuffers(std::array<FramebufferObject::Attachment, 1>({ FramebufferObject::Attachment::Color0 }));
    }
    Texture2DObject::Unbind();
    FramebufferObject::Unbind();
}

void PixelWavesApplication::InitializeRenderer()
{
    int width, height;
    GetMainWindow().GetDimensions(width, height);

    // Reflection pass
    {
        std::unique_ptr<GBufferRenderPass> gbufferRenderPass(std::make_unique<GBufferRenderPass>(width, height, 0, true, m_reflectionCamera));

        // Set the g-buffer textures as properties of the deferred material
        m_invDeferredMaterial->SetUniformValue("DepthTexture", gbufferRenderPass->GetDepthTexture());
        m_invDeferredMaterial->SetUniformValue("AlbedoTexture", gbufferRenderPass->GetAlbedoTexture());
        m_invDeferredMaterial->SetUniformValue("NormalTexture", gbufferRenderPass->GetNormalTexture());
        m_invDeferredMaterial->SetUniformValue("OthersTexture", gbufferRenderPass->GetOthersTexture());

        // Get the depth texture from the gbuffer pass - This could be reworked
        m_reflectDepthTexture = gbufferRenderPass->GetDepthTexture();

        // Add the render passes
        m_renderer.AddRenderPass(std::move(gbufferRenderPass));
        m_renderer.AddRenderPass(std::make_unique<DeferredRenderPass>(m_invDeferredMaterial, m_reflectSceneFramebuffer, true));

        // Scene Texture
        m_reflectSceneTexture = std::make_shared<Texture2DObject>();
        m_reflectSceneTexture->Bind();
        m_reflectSceneTexture->SetImage(0, width, height, TextureObject::FormatRGBA, TextureObject::InternalFormat::InternalFormatRGBA16F);
        m_reflectSceneTexture->SetParameter(TextureObject::ParameterEnum::WrapS, GL_CLAMP_TO_EDGE);
        m_reflectSceneTexture->SetParameter(TextureObject::ParameterEnum::WrapT, GL_CLAMP_TO_EDGE);
        m_reflectSceneTexture->SetParameter(TextureObject::ParameterEnum::MinFilter, GL_NEAREST);
        m_reflectSceneTexture->SetParameter(TextureObject::ParameterEnum::MagFilter, GL_NEAREST);
        Texture2DObject::Unbind();

        // Reflect scene framebuffer
        m_reflectSceneFramebuffer->Bind();
        m_reflectSceneFramebuffer->SetTexture(FramebufferObject::Target::Draw, FramebufferObject::Attachment::Depth, *m_reflectDepthTexture);
        m_reflectSceneFramebuffer->SetTexture(FramebufferObject::Target::Draw, FramebufferObject::Attachment::Color0, *m_reflectSceneTexture);
        m_reflectSceneFramebuffer->SetDrawBuffers(std::array<FramebufferObject::Attachment, 1>({ FramebufferObject::Attachment::Color0 }));
        FramebufferObject::Unbind();
    }

    // Set up deferred pass ( Scene / Refraction )
    {
        std::unique_ptr<GBufferRenderPass> gbufferRenderPass(std::make_unique<GBufferRenderPass>(width, height));

        // Set the g-buffer textures as properties of the deferred material
        m_deferredMaterial->SetUniformValue("DepthTexture", gbufferRenderPass->GetDepthTexture());
        m_deferredMaterial->SetUniformValue("AlbedoTexture", gbufferRenderPass->GetAlbedoTexture());
        m_deferredMaterial->SetUniformValue("NormalTexture", gbufferRenderPass->GetNormalTexture());
        m_deferredMaterial->SetUniformValue("OthersTexture", gbufferRenderPass->GetOthersTexture());

        // Get the depth texture from the gbuffer pass - This could be reworked
        m_depthTexture = gbufferRenderPass->GetDepthTexture();

        // Add the render passes
        m_renderer.AddRenderPass(std::move(gbufferRenderPass));
        m_renderer.AddRenderPass(std::make_unique<DeferredRenderPass>(m_deferredMaterial, m_sceneFramebuffer));
        
        // Skybox pass
        m_renderer.AddRenderPass(std::make_unique<SkyboxRenderPass>(m_skyboxTexture));

        // Initialize the framebuffers and the textures they use
        InitializeFramebuffers();
    }

    // Water pass
    {
        m_renderer.AddRenderPass(std::make_unique<WaterRenderPass>(m_waterMaterial, m_waterMesh, m_reflectSceneTexture, m_reflectDepthTexture, m_sceneFramebuffer));
    }

    //Post-processing
    {
        std::shared_ptr<Material> copyMaterial = CreatePostFXMaterial("shaders/postfx/copy.frag", m_sceneTexture);

        // Create a copy pass from m_sceneTexture to the first temporary texture
        m_renderer.AddRenderPass(std::make_unique<PostFXRenderPass>(copyMaterial, m_tempFramebuffers[0]));

        // Final pass
        m_composeMaterial = CreatePostFXMaterial("shaders/postfx/compose.frag", m_sceneTexture);

        // Set uniform default value
        m_composeMaterial->SetUniformValue("Exposure", m_exposure);
        m_composeMaterial->SetUniformValue("Contrast", m_contrast);
        m_composeMaterial->SetUniformValue("HueShift", m_hueShift);
        m_composeMaterial->SetUniformValue("Saturation", m_saturation);
        m_composeMaterial->SetUniformValue("ColorFilter", m_colorFilter);
        m_composeMaterial->SetUniformValue("Pixelation", m_pixelation);
        m_composeMaterial->SetUniformValue("Downsampling", m_downsampling);
        m_composeMaterial->SetUniformValue("Sharpness", m_sharpness);

        m_renderer.AddRenderPass(std::make_unique<PostFXRenderPass>(m_composeMaterial, m_renderer.GetDefaultFramebuffer()));
    }
}

std::shared_ptr<Material> PixelWavesApplication::CreateCombineMaterial(const char* fragmentShaderPath, std::shared_ptr<Texture2DObject> sourceTexture)
{
    // We could keep this vertex shader and reuse it, but it looks simpler this way
    std::vector<const char*> vertexShaderPaths;
    vertexShaderPaths.push_back("shaders/version330.glsl");
    vertexShaderPaths.push_back("shaders/renderer/fullscreen.vert");
    Shader vertexShader = ShaderLoader(Shader::VertexShader).Load(vertexShaderPaths);

    std::vector<const char*> fragmentShaderPaths;
    fragmentShaderPaths.push_back("shaders/version330.glsl");
    fragmentShaderPaths.push_back("shaders/utils.glsl");
    fragmentShaderPaths.push_back(fragmentShaderPath);
    Shader fragmentShader = ShaderLoader(Shader::FragmentShader).Load(fragmentShaderPaths);

    std::shared_ptr<ShaderProgram> shaderProgramPtr = std::make_shared<ShaderProgram>();
    shaderProgramPtr->Build(vertexShader, fragmentShader);

    // Create material
    std::shared_ptr<Material> material = std::make_shared<Material>(shaderProgramPtr);
    material->SetUniformValue("SourceTexture", sourceTexture);

    return material;
}

std::shared_ptr<Material> PixelWavesApplication::CreatePostFXMaterial(const char* fragmentShaderPath, std::shared_ptr<Texture2DObject> sourceTexture)
{
    // We could keep this vertex shader and reuse it, but it looks simpler this way
    std::vector<const char*> vertexShaderPaths;
    vertexShaderPaths.push_back("shaders/version330.glsl");
    vertexShaderPaths.push_back("shaders/renderer/fullscreen.vert");
    Shader vertexShader = ShaderLoader(Shader::VertexShader).Load(vertexShaderPaths);

    std::vector<const char*> fragmentShaderPaths;
    fragmentShaderPaths.push_back("shaders/version330.glsl");
    fragmentShaderPaths.push_back("shaders/utils.glsl");
    fragmentShaderPaths.push_back(fragmentShaderPath);
    Shader fragmentShader = ShaderLoader(Shader::FragmentShader).Load(fragmentShaderPaths);

    std::shared_ptr<ShaderProgram> shaderProgramPtr = std::make_shared<ShaderProgram>();
    shaderProgramPtr->Build(vertexShader, fragmentShader);

    // Create material
    std::shared_ptr<Material> material = std::make_shared<Material>(shaderProgramPtr);
    material->SetUniformValue("SourceTexture", sourceTexture);
    
    return material;
}

Renderer::UpdateTransformsFunction PixelWavesApplication::GetFullscreenTransformFunction(std::shared_ptr<ShaderProgram> shaderProgramPtr) const
{
    // Get transform related uniform locations
    ShaderProgram::Location invViewMatrixLocation = shaderProgramPtr->GetUniformLocation("InvViewMatrix");
    ShaderProgram::Location invProjMatrixLocation = shaderProgramPtr->GetUniformLocation("InvProjMatrix");
    ShaderProgram::Location worldViewProjMatrixLocation = shaderProgramPtr->GetUniformLocation("WorldViewProjMatrix");

    // Return transform function
    return [=](const ShaderProgram& shaderProgram, const glm::mat4& worldMatrix, const Camera& camera, bool cameraChanged)
        {
            //if (cameraChanged)
            //{
                shaderProgram.SetUniform(invViewMatrixLocation, glm::inverse(camera.GetViewMatrix()));
                shaderProgram.SetUniform(invProjMatrixLocation, glm::inverse(camera.GetProjectionMatrix()));
            //}
            shaderProgram.SetUniform(worldViewProjMatrixLocation, camera.GetViewProjectionMatrix() * worldMatrix);
        };
}

void PixelWavesApplication::RenderGUI()
{
    m_imGui.BeginFrame();

    // Draw GUI for scene nodes, using the visitor pattern
    ImGuiSceneVisitor imGuiVisitor(m_imGui, "Scene");
    m_scene.AcceptVisitor(imGuiVisitor);

    // Draw GUI for camera controller
    m_cameraController.DrawGUI(m_imGui);

    if (auto window = m_imGui.UseWindow("Post FX"))
    {
        if (m_composeMaterial)
        {
            if (ImGui::DragFloat("Exposure", &m_exposure, 0.01f, 0.01f, 5.0f))
            {
                m_composeMaterial->SetUniformValue("Exposure", m_exposure);
            }

            ImGui::Separator();

            if (ImGui::SliderFloat("Contrast", &m_contrast, 0.5f, 1.5f))
            {
                m_composeMaterial->SetUniformValue("Contrast", m_contrast);
            }
            if (ImGui::SliderFloat("Hue Shift", &m_hueShift, -0.5f, 0.5f))
            {
                m_composeMaterial->SetUniformValue("HueShift", m_hueShift);
            }
            if (ImGui::SliderFloat("Saturation", &m_saturation, 0.0f, 2.0f))
            {
                m_composeMaterial->SetUniformValue("Saturation", m_saturation);
            }
            if (ImGui::SliderFloat("Sharpness", &m_sharpness, 0.0f, 4.0f))
            {
                m_composeMaterial->SetUniformValue("Sharpness", m_sharpness);
            }
            if (ImGui::ColorEdit3("Color Filter", &m_colorFilter[0]))
            {
                m_composeMaterial->SetUniformValue("ColorFilter", m_colorFilter);
            }

            ImGui::Separator();

            if (ImGui::RadioButton("1x", &m_downsampling, 1)) {
                m_composeMaterial->SetUniformValue("Downsampling", m_downsampling);
            }

            ImGui::SameLine();

            if (ImGui::RadioButton("2x", &m_downsampling, 2)) {
                m_composeMaterial->SetUniformValue("Downsampling", m_downsampling);
            }

            ImGui::SameLine();

            if (ImGui::RadioButton("3x", &m_downsampling, 3)) {
                m_composeMaterial->SetUniformValue("Downsampling", m_downsampling);
            }

            ImGui::SameLine();
            
            if (ImGui::RadioButton("4x", &m_downsampling, 4)) {
                m_composeMaterial->SetUniformValue("Downsampling", m_downsampling);
            }

            ImGui::SameLine();
            
            if (ImGui::RadioButton("5x", &m_downsampling, 5)) {
                m_composeMaterial->SetUniformValue("Downsampling", m_downsampling);
            }

            ImGui::SameLine();

            ImGui::Text("Downsampling");
        }
    }

    if (auto window = m_imGui.UseWindow("Distortion"))
    {
        if (m_waterMaterial)
        {
            if (ImGui::DragFloat("Speed", &m_distortionSpeed, 0.1f, 1.0f, 10.0f))
            {
                m_waterMaterial->SetUniformValue("DistortionSpeed", m_distortionSpeed);
            }

            if (ImGui::DragFloat("Frequency", &m_distortionFrequency, 0.001f, 0.1f, 1.0f))
            {
                m_waterMaterial->SetUniformValue("DistortionFrequency", m_distortionFrequency);
            }

            if (ImGui::DragFloat("Strength", &m_distortionStrength, 0.0001f, 0.0f, 0.1f))
            {
                m_waterMaterial->SetUniformValue("DistortionStrength", m_distortionStrength);
            }
        }
    }

    m_imGui.EndFrame();
}
