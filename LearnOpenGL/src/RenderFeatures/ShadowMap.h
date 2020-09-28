#pragma once
#include "RenderFeature.h"
#include <memory>
#include <glm/glm.hpp>

class Shader;
class Model;
class Texture;

class RenderFeature_ShadowMap : public RenderFeatureBase
{
public:
	RenderFeature_ShadowMap(Camera* InCam) : RenderFeatureBase(InCam) {}

	void Setup() override;
	void Render() override;

private:
	void RenderQuad();

	std::shared_ptr<Shader> m_ShaderDepth;
	std::shared_ptr<Shader> m_ShaderShadow;
	std::shared_ptr<Shader> m_ShaderDebug;
	std::shared_ptr<Model> m_ModelCube;
	std::shared_ptr<Model> m_ModelFloor;
	std::shared_ptr<Texture> m_Texture;
	glm::vec3 m_PosLight;
	unsigned int m_FBO_DepthMap;
	unsigned int m_DepthMap;
	unsigned int m_VAO = 0;
	unsigned int m_VBO;
	const unsigned int SHADOW_WIDTH = 1024;
	const unsigned int SHADOW_HEIGHT = 1024;
};