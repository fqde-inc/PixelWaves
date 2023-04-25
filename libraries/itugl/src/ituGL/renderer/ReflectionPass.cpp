#include <ituGL/renderer/ReflectionPass.h>

#include <ituGL/renderer/Renderer.h>
#include <ituGL/camera/Camera.h>
#include <ituGL/scene/SceneCamera.h>
#include <ituGL/scene/Transform.h>
#include <ituGL/shader/Material.h>
#include <ituGL/texture/FramebufferObject.h>
#include <glm/gtc/matrix_transform.hpp>

ReflectionPass::ReflectionPass(std::shared_ptr <Camera> camera,  std::shared_ptr<const FramebufferObject> framebuffer)
    : RenderPass(framebuffer), m_camera(camera)
{
}

void ReflectionPass::Render()
{
    Renderer& renderer = GetRenderer();

    //Transform& transform = *m_camera->GetTransform();

    //glm::vec3 rotation = transform.GetRotation();
    //transform.SetRotation(glm::vec3(-rotation.x, rotation.y, rotation.z));

    //glm::vec3 translation = transform.GetTranslation();
    //transform.SetTranslation(glm::vec3(translation.x, -translation.y, translation.z));

    //renderer.SetCurrentCamera(*m_camera->GetCamera());

    //auto viewMatrix = m_camera->GetViewMatrix();
    //auto rot = m_camera->ExtractRotation();
    //auto pos = m_camera->ExtractTranslation();

    // Invert pitch by negating the x-axis rotation
    //auto invViewMatrix = glm::rotate(viewMatrix, -rot.x, glm::vec3(1.0f, 0.0f, 0.0f));

    // Negate y position to move the camera in the opposite direction along the y-axis
    //invViewMatrix = glm::translate(viewMatrix, glm::vec3(0.0f, -pos.y, 0.0f));

    //m_camera->SetViewMatrix(invViewMatrix);
    renderer.SetCurrentCamera(*m_camera);

    //assert(m_material);
    //m_material->Use();

    //const Mesh* mesh = &renderer.GetFullscreenMesh();
    //mesh->DrawSubmesh(0);

    //transform.SetRotation(rotation);
    //transform.SetTranslation(translation);

    //camera.SetViewMatrix(viewMatrix);
    //renderer.SetCurrentCamera(camera);

}
