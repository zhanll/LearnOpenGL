#include "Blend.h"
#include "../Camera.h"
#include "../Shader.h"
#include "../Model.h"
#include <glm/gtc/matrix_transform.hpp>

RenderFeature_Blend::RenderFeature_Blend(Camera* InCam)
	: RenderFeatureBase(InCam)
{
	m_Positions =
	{
		glm::vec3(-1.5f, 0.0f, -0.48f),
		glm::vec3(1.5f, 0.0f, 0.51f),
		glm::vec3(0.0f, 0.0f, 0.7f),
		glm::vec3(-0.3f, 0.0f, -2.3f),
		glm::vec3(0.5f, 0.0f, -0.6f)
	};
}

void RenderFeature_Blend::Setup()
{
	m_ShaderGrass = std::make_shared<Shader>("res/shaders/basic.vs", "res/shaders/transparent.fs");
	m_ModelGrass = std::make_shared<Model>();
	m_ModelGrass->SetTextureWrapS(GL_CLAMP_TO_EDGE);
	m_ModelGrass->SetTextureWrapT(GL_CLAMP_TO_EDGE);
	m_ModelGrass->loadModel("res/models/grass/plane.obj");
}

void RenderFeature_Blend::Render()
{
	RenderFeatureBase::Render();

	/** grass */
	m_ShaderGrass->use();
	m_ShaderGrass->setMat4("view", m_MatView);
	m_ShaderGrass->setMat4("projection", m_MatProjection);

	for (size_t i = 0; i < m_Positions.size(); i++)
	{
		glm::mat4 grassMat = glm::mat4(1.0f);
		grassMat = glm::translate(grassMat, m_Positions[i]);
		grassMat = glm::scale(grassMat, glm::vec3(3.0f));
		grassMat = glm::rotate(grassMat, glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));

		m_ShaderGrass->setMat4("model", grassMat);

		m_ModelGrass->Draw(*m_ShaderGrass);
	}
}
