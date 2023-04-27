#pragma once

#include <ituGL/renderer/RenderPass.h>

#include <ituGL/shader/ShaderProgram.h>
#include <ituGL/geometry/Mesh.h>
#include <memory>

class Texture2DObject;
class Material;

class DeferredRenderPass: public RenderPass
{
public:
    DeferredRenderPass(std::shared_ptr<Material> material, std::shared_ptr<const FramebufferObject> targetFramebuffer = nullptr, bool reflectionPass = false);

    void Render() override;

private:
    void InitializeMeshes();

private:
    bool m_reflectionPass;

    std::shared_ptr<Material> m_material;
};
