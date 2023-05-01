#include <ituGL/renderer/WaterRenderPass.h>

#include <ituGL/renderer/Renderer.h>
#include <ituGL/geometry/Drawcall.h>
#include <ituGL/shader/Material.h>
#include <ituGL/texture/FramebufferObject.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

WaterRenderPass::WaterRenderPass (std::shared_ptr<Material> material, std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture2DObject> sceneTexture, std::shared_ptr<Texture2DObject> depthTexture, std::shared_ptr<const FramebufferObject> framebuffer)
    : RenderPass(framebuffer), 
    m_material(material), 
    m_mesh(mesh), 
    m_sceneTexture(sceneTexture), 
    m_depth(depthTexture)
{
}

void WaterRenderPass::Render()
{
    Renderer& renderer = GetRenderer();

    //glEnable(GL_DEPTH_TEST);

    assert(m_material);
    assert(m_material->GetDepthWrite());

    m_material->SetUniformValue("DepthSampler", m_depth);
    m_material->SetUniformValue("SceneTexture", m_sceneTexture);


    Renderer::DrawcallInfo drawcallInfo (
        *m_material.get(),
        0,
        m_mesh->GetSubmeshVertexArray(0),
        m_mesh->GetSubmeshDrawcall(0)
    );

    renderer.PrepareDrawcall(drawcallInfo);

    drawcallInfo.drawcall.Draw();

}
