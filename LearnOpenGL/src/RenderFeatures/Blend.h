#pragma once
#include "RenderFeature.h"
#include <memory>
#include <vector>
#include <glm/glm.hpp>

class Shader;
class Model;

class RenderFeature_Blend : public RenderFeatureBase
{
public:
	RenderFeature_Blend(Camera* InCam);

	void Setup() override;
	void Render() override;

private:
	std::shared_ptr<Shader> m_ShaderGrass;
	std::shared_ptr<Model> m_ModelGrass;
	std::vector<glm::vec3> m_Positions;
};