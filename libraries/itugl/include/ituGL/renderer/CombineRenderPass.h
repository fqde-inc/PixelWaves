#pragma once

#include <ituGL/renderer/RenderPass.h>
#include <ituGL/renderer/Renderer.h>
#include <ituGL/texture/Texture2DObject.h>

#include <memory>
#include <ituGL/geometry/Mesh.h>

class Material;
class FramebufferObject;

class CombineRenderPass: public RenderPass
{
public:
    CombineRenderPass(std::shared_ptr<Material> combineMaterial, std::shared_ptr<Texture2DObject> inputTexture1, std::shared_ptr<Texture2DObject> m_inputTexture2, std::shared_ptr<const FramebufferObject> targetFramebuffer = nullptr);

    void Render() override;

private:
    std::shared_ptr<Material> m_combineMaterial;

    std::shared_ptr<Texture2DObject> m_inputTexture1;
    std::shared_ptr<Texture2DObject> m_inputTexture2;

    std::shared_ptr<FramebufferObject> m_framebuffer;
};