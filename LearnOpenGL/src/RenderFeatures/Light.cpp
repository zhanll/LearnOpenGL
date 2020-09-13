#include "Light.h"
#include "../Camera.h"
#include "../Shader.h"
#include "../Model.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

RenderFeature_Light::RenderFeature_Light(Camera* InCam)
	: RenderFeatureBase(InCam)
{
	m_PositionLights =
	{
		glm::vec3(5.f, 0.f, 0.f),
		glm::vec3(0.f, 5.f, 0.f),
		glm::vec3(0.f, 0.f, 5.f),
		glm::vec3(-5.f, 0.f, 0.f),
		glm::vec3(0.f, -5.f, 0.f),
		glm::vec3(0.f, 0.f, -5.f)
	};

	m_PositionContainers =
	{
		glm::vec3(2.f, 0.f, 0.f),
		glm::vec3(0.f, 2.f, 0.f),
		glm::vec3(0.f, 0.f, 2.f),
		glm::vec3(-2.f, 0.f, 0.f),
		glm::vec3(0.f, -2.f, 0.f),
		glm::vec3(0.f, 0.f, -2.f)
	};
}

void RenderFeature_Light::Setup()
{
	m_ShaderLight = std::make_shared<Shader>("res/shaders/basic.vs", "res/shaders/light_cube.fs");
	m_ShaderContainer = std::make_shared<Shader>("res/shaders/light_combination.vs", "res/shaders/light_combination.fs");
	m_ModelLight = std::make_shared<Model>("res/models/cube/cube.obj");
	m_ModelContainer = std::make_shared<Model>("res/models/container/cube.obj");
}

void RenderFeature_Light::Render()
{
	RenderFeatureBase::Render();

	float time = (float)glfwGetTime();
	float sint = glm::sin(time);

	/* Lights */
	m_ShaderLight->use();
	m_ShaderLight->setMat4("view", m_MatView);
	m_ShaderLight->setMat4("projection", m_MatProjection);

	for (size_t iLight = 0; iLight < m_PositionLights.size(); iLight++)
	{
		glm::mat4 MatModel(1.f);
		MatModel = glm::rotate(MatModel, glm::radians(sint * 180), glm::vec3( (float)((iLight - 1) % 2), (float)(iLight % 2), (float)((iLight + 1) % 2) ));
		MatModel = glm::translate(MatModel, m_PositionLights[iLight]);
		m_ShaderLight->use();
		m_ShaderLight->setMat4("model", MatModel);
		m_ModelLight->Draw(*m_ShaderLight);

		glm::vec3 LightPos = glm::vec3(MatModel * glm::vec4(m_PositionLights[iLight], 1.f));

		std::ostringstream ossHead;
		ossHead << "pointLight[" << iLight;
		std::string strHead = ossHead.str();

		m_ShaderContainer->use();
		m_ShaderContainer->setVec3(strHead + "].position", LightPos);
		m_ShaderContainer->setVec3(strHead + "].ambient", 0.05f, 0.05f, 0.05f);
		m_ShaderContainer->setVec3(strHead + "].diffuse", 0.5f, 0.5f, 0.5f);
		m_ShaderContainer->setVec3(strHead + "].specular", 1.0f, 1.0f, 1.0f);
		m_ShaderContainer->setFloat(strHead + "].constant", 1.0f);
		m_ShaderContainer->setFloat(strHead + "].linear", 0.09f);
		m_ShaderContainer->setFloat(strHead + "].quadratic", 0.032f);
	}

	/* Containers */
	m_ShaderContainer->use();

	m_ShaderContainer->setFloat("material.shininess", 32.0f);

	m_ShaderContainer->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
	m_ShaderContainer->setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
	m_ShaderContainer->setVec3("dirLight.diffuse", 0.3f, 0.3f, 0.3f);
	m_ShaderContainer->setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

	m_ShaderContainer->setVec3("spotLight.position", m_Camera->GetCameraPos());
	m_ShaderContainer->setVec3("spotLight.direction", m_Camera->GetCameraDirection());
	m_ShaderContainer->setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	m_ShaderContainer->setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
	m_ShaderContainer->setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
	m_ShaderContainer->setFloat("spotLight.constant", 1.0f);
	m_ShaderContainer->setFloat("spotLight.linear", 0.09f);
	m_ShaderContainer->setFloat("spotLight.quadratic", 0.032f);
	m_ShaderContainer->setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	m_ShaderContainer->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));

	m_ShaderContainer->setVec3("viewPos", m_Camera->GetCameraPos());

	m_ShaderContainer->setMat4("view", m_MatView);
	m_ShaderContainer->setMat4("projection", m_MatProjection);

	for (size_t iContainer = 0; iContainer < m_PositionContainers.size(); iContainer++)
	{
		glm::mat4 MatModel(1.f);
		MatModel = glm::translate(MatModel, m_PositionContainers[iContainer]);
		MatModel = glm::rotate(MatModel, glm::radians(sint * 180), glm::vec3((float)((iContainer - 1) % 2), (float)(iContainer % 2), (float)((iContainer + 1) % 2)));
		
		m_ShaderContainer->setMat4("model", MatModel);
		m_ModelContainer->Draw(*m_ShaderContainer);
	}
}