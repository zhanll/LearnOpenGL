#pragma once
#include "glm/glm.hpp"

class Camera;

class RenderFeatureBase
{
public:
	RenderFeatureBase(Camera* InCam) : m_Camera(InCam) {}
	virtual void Setup() {};
	virtual void Render();

protected:
	Camera* m_Camera = nullptr;
	glm::mat4 m_MatView;
	glm::mat4 m_MatProjection;
};