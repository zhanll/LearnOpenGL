#pragma once
#include "RenderFeature.h"
#include <memory>
#include <vector>
#include <string>

class Shader;
class Model;

class RenderFeature_Skybox : public RenderFeatureBase
{
public:
	RenderFeature_Skybox(Camera* InCam) : RenderFeatureBase(InCam) {};
	void Setup() override;
	void Render() override;

private:
	unsigned int LoadCubemap(std::vector<std::string> faces);

	std::shared_ptr<Shader> m_Shader;
	std::shared_ptr<Model> m_Model;
	unsigned int m_CubemapTexture;
};