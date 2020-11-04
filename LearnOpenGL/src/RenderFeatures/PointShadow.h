#pragma once
#include "RenderFeature.h"
#include <memory>
#include <glm/glm.hpp>

class Shader;
class Model;
class Texture;

class RenderFeature_PointShadow :
    public RenderFeatureBase
{
public:
    RenderFeature_PointShadow(Camera* InCam) : RenderFeatureBase(InCam) {}
    void Setup() override;
    void Render() override;

private:
    void RenderScene(std::shared_ptr<Shader> InShader);

    std::shared_ptr<Shader> m_shaderDepth;
    std::shared_ptr<Shader> m_shaderScene;
    std::shared_ptr<Shader> m_shaderLight;
    std::shared_ptr<Shader> m_shaderDebug;
    std::shared_ptr<Model> m_modelFloor;
    std::shared_ptr<Model> m_modelContainer;
    std::shared_ptr<Model> m_modelLight;
    std::shared_ptr<Texture> m_Texture;

    unsigned int m_depthCubemap;
    unsigned int m_depthMapFBO;
	unsigned int m_VAO;
	unsigned int m_VBO;

    glm::vec3 m_lightPos;
};

