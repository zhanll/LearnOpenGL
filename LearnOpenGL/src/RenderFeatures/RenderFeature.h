#pragma once

class Camera;

class RenderFeatureBase
{
public:
	RenderFeatureBase(Camera* InCam) : m_Camera(InCam) {}
	virtual void Setup() {};
	virtual void Render() {};

protected:
	Camera* m_Camera = nullptr;
};