#pragma once

#include <ituGL/renderer/RenderPass.h>
#include <ituGL/camera/Camera.h>

#include <memory>

class Material;
class FramebufferObject;

class ReflectionPass: public RenderPass
{
public:
    ReflectionPass(std::shared_ptr<Camera> camera, std::shared_ptr<const FramebufferObject> targetFramebuffer = nullptr);

    void Render() override;

private:
    std::shared_ptr<Camera> m_camera;
    std::shared_ptr<FramebufferObject> m_framebuffer;
};
