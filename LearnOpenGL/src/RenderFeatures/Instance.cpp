#include "Instance.h"
#include "../Shader.h"
#include "../Model.h"
#include "../Camera.h"
#include "../Config.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

unsigned int amount = 100000;

void RenderFeature_Instance::Setup()
{
	m_ShaderAsteroid = std::make_shared<Shader>("res/shaders/asteroids.vs", "res/shaders/basic.fs");
	m_ShaderPlanet = std::make_shared<Shader>("res/shaders/basic.vs", "res/shaders/basic.fs");
	m_ModelAsteriod = std::make_shared<Model>("res/models/rock/rock.obj");
	m_ModelPlanet = std::make_shared<Model>("res/models/planet/planet.obj");

	m_Camera->SetCameraPos(glm::vec3(0.f, 0.f, 105.f));

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

	// set transformation matrices as an instance vertex attribute (with divisor 1)
	// note: we're cheating a little by taking the, now publicly declared, VAO of the model's mesh(es) and adding new vertexAttribPointers
	// normally you'd want to do this in a more organized fashion, but for learning purposes this will do.
	// -----------------------------------------------------------------------------------------------------------------------------------
	for (unsigned int i = 0; i < m_ModelAsteriod->GetMeshes().size(); i++)
	{
		unsigned int VAO = m_ModelAsteriod->GetMeshes()[i].GetVAO();
		glBindVertexArray(VAO);
		// set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}

	delete[] modelMatrices;
}

void RenderFeature_Instance::Render()
{
	// configure transformation matrices
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 1000.0f);
	glm::mat4 view = m_Camera->GetViewMatrix();

	m_ShaderAsteroid->use();
	m_ShaderAsteroid->setMat4("projection", projection);
	m_ShaderAsteroid->setMat4("view", view);

	m_ShaderPlanet->use();
	m_ShaderPlanet->setMat4("projection", projection);
	m_ShaderPlanet->setMat4("view", view);

	// draw planet
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
	model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
	m_ShaderPlanet->setMat4("model", model);
	m_ModelPlanet->Draw(*m_ShaderPlanet);

	// draw meteorites
	m_ShaderAsteroid->use();
	m_ShaderAsteroid->setInt("texture_diffuse1", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_ModelAsteriod->GetLoadedTextures()[0].GetCorrectID());
	for (unsigned int i = 0; i < m_ModelAsteriod->GetMeshes().size(); i++)
	{
		glBindVertexArray((m_ModelAsteriod->GetMeshes()[i]).GetVAO());
		glDrawElementsInstanced(GL_TRIANGLES, m_ModelAsteriod->GetMeshes()[i].indices.size(), GL_UNSIGNED_INT, 0, amount);
		glBindVertexArray(0);
	}
}
