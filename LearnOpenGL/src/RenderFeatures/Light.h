#pragma once
#include "RenderFeature.h"
#include <memory>
#include <vector>
#include "glm/glm.hpp"

class Camera;
class Shader;
class Model;

class RenderFeature_Light : public RenderFeatureBase
{
public:
	RenderFeature_Light(Camera* InCam);

	void Setup() override;
	void Render() override;

private:
	std::shared_ptr<Shader> m_ShaderLight;
	std::shared_ptr<Shader> m_ShaderContainer;
	std::shared_ptr<Model> m_ModelLight;
	std::shared_ptr<Model> m_ModelContainer;
	std::vector<glm::vec3> m_PositionLights;
	std::vector<glm::vec3> m_PositionContainers;
};