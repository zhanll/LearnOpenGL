#pragma once
#include "RenderFeature.h"
#include <memory>

class Shader;
class Model;

class RenderFeature_FrameBuffer : public RenderFeatureBase
{
public:
	RenderFeature_FrameBuffer(Camera* InCam) : RenderFeatureBase(InCam) {}
	~RenderFeature_FrameBuffer();

	void Setup() override;
	void Render() override;

private:
	std::shared_ptr<Shader> m_ShaderBasic;
	std::shared_ptr<Shader> m_ShaderScreen;
	std::shared_ptr<Model> m_Model;
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_FBO;
	unsigned int m_CBO;
	unsigned int m_RBO;
};