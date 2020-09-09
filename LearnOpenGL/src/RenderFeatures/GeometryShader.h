#pragma once
#include "RenderFeature.h"
#include <memory>

class Shader;
class Model;

class RenderFeature_GeometryShader : public RenderFeatureBase
{
public:
	RenderFeature_GeometryShader(Camera* InCam) : RenderFeatureBase(InCam) {};
	void Setup() override;
	void Render() override;

private:
	std::shared_ptr<Shader> m_ShaderExplode;
	std::shared_ptr<Shader> m_ShaderNormalVis;
	std::shared_ptr<Model> m_Model;
};