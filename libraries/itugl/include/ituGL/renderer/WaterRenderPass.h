#pragma once

#include <ituGL/renderer/RenderPass.h>
#include <ituGL/renderer/Renderer.h>
#include <ituGL/texture/Texture2DObject.h>

#include <memory>
#include <ituGL/geometry/Mesh.h>

class Material;
class FramebufferObject;

class WaterRenderPass: public RenderPass
{
public:
    WaterRenderPass(std::shared_ptr<Material> material, std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture2DObject> sceneTexture, glm::vec4 sceneColor, std::shared_ptr<const FramebufferObject> targetFramebuffer = nullptr);

    void Render() override;

private:
    std::shared_ptr<Texture2DObject> m_sceneTexture;
    glm::vec4 m_sceneColor;
    std::shared_ptr<Material> m_material;
    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<FramebufferObject> m_framebuffer;
};