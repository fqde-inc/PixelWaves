#include <ituGL/renderer/CombineRenderPass.h>

#include <ituGL/renderer/Renderer.h>
#include <ituGL/geometry/Drawcall.h>
#include <ituGL/shader/Material.h>
#include <ituGL/texture/FramebufferObject.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

CombineRenderPass::CombineRenderPass (std::shared_ptr<Material> material, std::shared_ptr<Texture2DObject> texture1, std::shared_ptr<Texture2DObject> texture2, std::shared_ptr<const FramebufferObject> framebuffer)
    : RenderPass(framebuffer), 
    m_combineMaterial(material),
    m_inputTexture1(),
    m_inputTexture2()
{
}

void CombineRenderPass::Render()
{
    Renderer& renderer = GetRenderer();

    //glEnable(GL_DEPTH_TEST);

    assert(m_combineMaterial);

    m_combineMaterial->SetUniformValue("Texture1", m_inputTexture1);
    m_combineMaterial->SetUniformValue("Texture2", m_inputTexture2);

    assert(m_combineMaterial);
    m_combineMaterial->Use();

    const Mesh* mesh = &renderer.GetFullscreenMesh();
    mesh->DrawSubmesh(0);

}
