#include "GeometryShader.h"
#include "../Shader.h"
#include "../Model.h"
#include "../Camera.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>


void RenderFeature_GeometryShader::Setup()
{
	m_ShaderExplode = std::make_shared<Shader>("res/shaders/explode.vs", "res/shaders/explode.fs", "res/shaders/explode.gs");
	m_ShaderNormalVis = std::make_shared<Shader>("res/shaders/normal_visualize.vs", "res/shaders/normal_visualize.fs", "res/shaders/normal_visualize.gs");
	m_Model = std::make_shared<Model>("res/models/backpack/backpack.obj");
}

void RenderFeature_GeometryShader::Render()
{
	RenderFeatureBase::Render();

	if (!m_Camera)
	{
		return;
	}

	m_ShaderExplode->use();
	m_ShaderExplode->setMat4("view", m_MatView);
	m_ShaderExplode->setMat4("projection", m_MatProjection);

	glm::mat4 backpackMat = glm::mat4(1.0f);
	backpackMat = glm::translate(backpackMat, glm::vec3(0.f, 2.f, -5.f));
	m_ShaderExplode->setMat4("model", backpackMat);
	m_ShaderExplode->setFloat("time", (float)glfwGetTime());
	m_Model->Draw(*m_ShaderExplode);

	m_ShaderNormalVis->use();
	m_ShaderNormalVis->setMat4("view", m_MatView);
	m_ShaderNormalVis->setMat4("projection", m_MatProjection);
	m_ShaderNormalVis->setMat4("model", backpackMat);
	m_Model->Draw(*m_ShaderNormalVis);
}
