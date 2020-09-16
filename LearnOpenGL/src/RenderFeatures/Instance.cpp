#include "Instance.h"
#include "../Shader.h"
#include "../Model.h"
#include "../Config.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

unsigned int amount = 100000;

void RenderFeature_Instance::Setup()
{
	m_ShaderAsteriod = std::make_shared<Shader>("res/shaders/asteriods.vs", "res/shaders/basic.fs");
	m_ShaderPlanet = std::make_shared<Shader>("res/shaders/basic.vs", "res/shaders/basic.fs");
	m_ModelAsteriod = std::make_shared<Model>("res/models/rock/m_ModelAsteriod->obj");
	m_ModelPlanet = std::make_shared<Model>("res/models/planet/planet.obj");

	// generate a large list of semi-random model transformation matrices
	// ------------------------------------------------------------------
	glm::mat4* modelMatrices;
	modelMatrices = new glm::mat4[amount];
	srand(glfwGetTime()); // initialize random seed	
	float radius = 150.0;
	float offset = 25.0f;
	for (unsigned int i = 0; i < amount; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		// 1. translation: displace along circle with 'radius' in range [-offset, offset]
		float angle = (float)i / (float)amount * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		// 2. scale: Scale between 0.05 and 0.25f
		float scale = (rand() % 20) / 100.0f + 0.05;
		model = glm::scale(model, glm::vec3(scale));

		// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
		float rotAngle = (rand() % 360);
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		// 4. now add to list of matrices
		modelMatrices[i] = model;
	}

	// configure instanced array
	// -------------------------
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	delete [] modelMatrices;
}

void RenderFeature_Instance::Render()
{
	RenderFeatureBase::Render();

	// configure transformation matrices
	m_ShaderAsteriod->use();
	m_ShaderAsteriod->setMat4("projection", m_MatProjection);
	m_ShaderAsteriod->setMat4("view", m_MatView);

	m_ShaderPlanet->use();
	m_ShaderPlanet->setMat4("projection", m_MatProjection);
	m_ShaderPlanet->setMat4("view", m_MatView);

	// draw planet
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
	model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
	m_ShaderPlanet->setMat4("model", model);
	m_ModelPlanet->Draw(*m_ShaderPlanet);

	// draw meteorites
	m_ShaderAsteriod->use();
	m_ShaderAsteriod->setInt("texture_diffuse1", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_ModelAsteriod->GetLoadedTextures()[0].id);
	for (unsigned int i = 0; i < m_ModelAsteriod->GetMeshes().size(); i++)
	{
		glBindVertexArray((m_ModelAsteriod->GetMeshes()[i]).GetVAO());
		glDrawElementsInstanced(GL_TRIANGLES, m_ModelAsteriod->GetMeshes()[i].indices.size(), GL_UNSIGNED_INT, 0, amount);
		glBindVertexArray(0);
	}
}
