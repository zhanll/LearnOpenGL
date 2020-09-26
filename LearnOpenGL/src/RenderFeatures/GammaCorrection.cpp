#include "GammaCorrection.h"
#include "../Shader.h"
#include "../Texture.h"
#include "../Camera.h"
#include "../Model.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

RenderFeature_GammaCorrection::~RenderFeature_GammaCorrection()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
}

void RenderFeature_GammaCorrection::Setup()
{
	m_ShaderFloor = std::make_shared<Shader>("res/shaders/light_combination.vs", "res/shaders/gamma_correction.fs");
	m_ShaderLight = std::make_shared<Shader>("res/shaders/basic.vs", "res/shaders/light_cube.fs");
	m_ModelLight = std::make_shared<Model>("res/models/cube/cube.obj");
	m_Texture = std::make_shared<Texture>("texture_diffuse", "res/textures/wall.jpg");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float planeVertices[] = {
		// positions            // normals         // texcoords
		 10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
		-10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		-10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

		 10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
		-10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
		 10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
	};
	// plane VAO
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glBindVertexArray(0);

	// load textures
	// -------------
	m_Texture->LoadFromFile(true);
	m_Texture->LoadFromFile(false);

	// lighting info
	// -------------
	m_PositionLights =
	{
		glm::vec3(5.f, 0.f, 0.f),
		glm::vec3(0.f, 5.f, 0.f),
		glm::vec3(0.f, 0.f, 5.f),
		glm::vec3(-5.f, 0.f, 0.f),
		glm::vec3(0.f, -5.f, 0.f),
		glm::vec3(0.f, 0.f, -5.f)
	};

	m_Camera->SetCameraPos(glm::vec3(0.f, 0.f, 3.f));
	m_Camera->SetCameraYaw(0.f);

	glDisable(GL_CULL_FACE);
}

void RenderFeature_GammaCorrection::Render()
{
	RenderFeatureBase::Render();

	/* Lights */
	m_ShaderLight->use();
	m_ShaderLight->setMat4("view", m_MatView);
	m_ShaderLight->setMat4("projection", m_MatProjection);

	for (size_t iLight = 0; iLight < m_PositionLights.size(); iLight++)
	{
		glm::mat4 MatModelLight(1.f);
		MatModelLight = glm::translate(MatModelLight, m_PositionLights[iLight]);
		//glm::vec3 LightPos = glm::vec3(MatModelLight * glm::vec4(m_PositionLights[iLight], 1.f));
		m_ShaderLight->use();
		m_ShaderLight->setMat4("model", MatModelLight);
		m_ModelLight->Draw(*m_ShaderLight);

		std::ostringstream ossHead;
		ossHead << "pointLight[" << iLight;
		std::string strHead = ossHead.str();

		m_ShaderFloor->use();
		m_ShaderFloor->setVec3(strHead + "].position", m_PositionLights[iLight]);
		m_ShaderFloor->setVec3(strHead + "].ambient", 0.05f, 0.05f, 0.05f);
		m_ShaderFloor->setVec3(strHead + "].diffuse", 0.5f, 0.5f, 0.5f);
		m_ShaderFloor->setVec3(strHead + "].specular", 1.0f, 1.0f, 1.0f);
		m_ShaderFloor->setFloat(strHead + "].constant", 1.0f);
		m_ShaderFloor->setFloat(strHead + "].linear", 0.09f);
		m_ShaderFloor->setFloat(strHead + "].quadratic", 0.032f);
	}

	m_ShaderFloor->setFloat("material.shininess", 32.0f);
	m_ShaderFloor->setVec3("viewPos", m_Camera->GetCameraPos());
	m_ShaderFloor->setBool("gamma", m_GammaCorrection);

	m_ShaderFloor->setMat4("view", m_MatView);
	m_ShaderFloor->setMat4("projection", m_MatProjection);

	glm::mat4 MatModel(1.f);
	m_ShaderFloor->setMat4("model", MatModel);

	// floor
	glBindVertexArray(m_VAO);
	glActiveTexture(GL_TEXTURE0);
	m_Texture->SetSRGB(m_GammaCorrection);
	glBindTexture(GL_TEXTURE_2D, m_Texture->GetCorrectID());
	glDrawArrays(GL_TRIANGLES, 0, 6);

	std::cout << (m_GammaCorrection ? "Gamma enabled" : "Gamma disabled") << std::endl;
}

void RenderFeature_GammaCorrection::SetGammaCorrection(bool bEnable)
{
	m_GammaCorrection = bEnable;
}
