#pragma once
#include "RenderFeature.h"
#include <memory>

class Shader;
class Model;

class RenderFeature_Instance : public RenderFeatureBase
{
public:
	RenderFeature_Instance(Camera* InCam) : RenderFeatureBase(InCam) {}

	void Setup() override;
	void Render() override;

protected:
	std::shared_ptr<Shader> m_ShaderAsteriod;
	std::shared_ptr<Shader> m_ShaderPlanet;
	std::shared_ptr<Model> m_ModelAsteriod;
	std::shared_ptr<Model> m_ModelPlanet;
};