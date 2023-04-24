#pragma once

#include <ituGL/renderer/RenderPass.h>
#include <ituGL/camera/CameraController.h>

#include <memory>

class Material;
class FramebufferObject;

class ReflectionPass: public RenderPass
{
public:
    ReflectionPass(std::shared_ptr<Material> material, std::shared_ptr<SceneCamera> camera, std::shared_ptr<const FramebufferObject> targetFramebuffer = nullptr);

    void Render() override;

private:
    std::shared_ptr<SceneCamera> m_camera;
    std::shared_ptr<Material> m_material;
    std::shared_ptr<FramebufferObject> m_framebuffer;
};
