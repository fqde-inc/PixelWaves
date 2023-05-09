#pragma once

#include <ituGL/renderer/RenderPass.h>
#include <ituGL/camera/Camera.h>

class Texture2DObject;

class GBufferRenderPass : public RenderPass
{
public:
    GBufferRenderPass(int width, int height, int drawcallCollectionIndex = 0, bool reflection = false, std::shared_ptr<Camera> targetCamera = nullptr);

    void Render() override;

    const std::shared_ptr<Texture2DObject> GetDepthTexture() const { return m_depthTexture; }
    const std::shared_ptr<Texture2DObject> GetAlbedoTexture() const { return m_albedoTexture; }
    const std::shared_ptr<Texture2DObject> GetNormalTexture() const { return m_normalTexture; }
    const std::shared_ptr<Texture2DObject> GetOthersTexture() const { return m_othersTexture; }

private:
    void InitTextures(int width, int height);
    void InitFramebuffer();

private:
    Camera m_tempCamera;
    std::shared_ptr<Camera> m_targetCamera;
    bool m_reflectionPass;

    int m_drawcallCollectionIndex;

    std::shared_ptr<Texture2DObject> m_depthTexture;
    std::shared_ptr<Texture2DObject> m_albedoTexture;
    std::shared_ptr<Texture2DObject> m_normalTexture;
    std::shared_ptr<Texture2DObject> m_othersTexture;
};
