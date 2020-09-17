#pragma once
#include "RenderFeature.h"
#include <memory>

class Shader;
class Model;

class RenderFeature_AntiAliasing : public RenderFeatureBase
{
public:
	RenderFeature_AntiAliasing(Camera* InCam) : RenderFeatureBase(InCam) {}

	void Setup() override;
	void Render() override;

private:
	std::shared_ptr<Shader> m_ShaderCube;
	std::shared_ptr<Model> m_ModelCube;

	unsigned int m_FBOMultiSampled;
	unsigned int m_CBOMultiSampled;
	unsigned int m_RBOMultiSampled;
};