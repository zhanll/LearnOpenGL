#pragma once
#include "RenderFeature.h"
#include <memory>
#include <vector>

class Shader;
class Model;
class Texture;

class RenderFeature_GammaCorrection : public RenderFeatureBase
{
public:
	RenderFeature_GammaCorrection(Camera* InCam) : RenderFeatureBase(InCam) {};
	~RenderFeature_GammaCorrection();

	void Setup() override;
	void Render() override;

	void SetGammaCorrection(bool bEnable);

private:
	std::shared_ptr<Shader> m_ShaderFloor;
	std::shared_ptr<Shader> m_ShaderLight;
	std::shared_ptr<Model> m_ModelLight;
	std::shared_ptr<Texture> m_Texture;
	std::vector<glm::vec3> m_PositionLights;
	unsigned int m_VAO;
	unsigned int m_VBO;
	bool m_GammaCorrection = false;
};