#include "ShadowMap.h"
#include "../Shader.h"
#include "../Model.h"
#include "../Camera.h"
#include "../Texture.h"
#include "../Config.h"
#include <glm/gtc/matrix_transform.hpp>

void RenderFeature_ShadowMap::Setup()
{
	m_ShaderDepth = std::make_shared<Shader>("res/shaders/depth_map.vs", "res/shaders/depth_map.fs");
	m_ShaderShadow = std::make_shared<Shader>("res/shaders/shadow.vs", "res/shaders/shadow.fs");
	m_ShaderDebug = std::make_shared<Shader>("res/shaders/depth_map_debug.vs", "res/shaders/depth_map_debug.fs");
	m_ModelCube = std::make_shared<Model>("res/models/container/cube.obj");
	m_ModelFloor = std::make_shared<Model>("res/models/plane/plane.obj");
	m_Texture = std::make_shared<Texture>("texture_diffuse", "res/textures/container.jpg");
	m_Texture->LoadFromFile();

	m_PosLight = glm::vec3(-2.0f, 4.0f, -1.0f);

	glGenFramebuffers(1, &m_FBO_DepthMap);

	glGenTextures(1, &m_DepthMap);
	glBindTexture(GL_TEXTURE_2D, m_DepthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO_DepthMap);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderFeature_ShadowMap::Render()
{
	//RenderFeatureBase::Render();

	// 1. first render to depth map
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO_DepthMap);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	float nearPlane = 1.f, farPlane = 7.5f;
	glm::mat4 lightProjection = glm::ortho(-10.f, 10.f, -10.f, 10.f, nearPlane, farPlane);
	glm::mat4 lightView = glm::lookAt(m_PosLight, glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	m_ShaderDepth->use();
	m_ShaderDepth->setMat4("lightSpaceMatrix", lightSpaceMatrix);

	glm::mat4 MatModelFloor(1.f);
	MatModelFloor = glm::translate(MatModelFloor, glm::vec3(0.f, -1.f, 0.f));
	MatModelFloor = glm::scale(MatModelFloor, glm::vec3(15.f, 1.f, 15.f));
	m_ShaderDepth->setMat4("model", MatModelFloor);
	m_ModelFloor->Draw(*m_ShaderDepth);

	glm::mat4 MatModelCube(1.f);
	MatModelCube = glm::translate(MatModelCube, glm::vec3(0.f, 1.5f, 0.f));
	m_ShaderDepth->setMat4("model", MatModelCube);
	m_ModelCube->Draw(*m_ShaderDepth);

	MatModelCube = glm::mat4(1.f);
	MatModelCube = glm::translate(MatModelCube, glm::vec3(2.f, 0.f, 1.f));
	m_ShaderDepth->setMat4("model", MatModelCube);
	m_ModelCube->Draw(*m_ShaderDepth);

	MatModelCube = glm::mat4(1.f);
	MatModelCube = glm::translate(MatModelCube, glm::vec3(-1.f, 0.f, 2.f));
	MatModelCube = glm::rotate(MatModelCube, glm::radians(60.f), glm::normalize(glm::vec3(1.f, 0.f, 1.f)));
	m_ShaderDepth->setMat4("model", MatModelCube);
	m_ModelCube->Draw(*m_ShaderDepth);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 2. then render scene as normal with shadow mapping (using depth map)
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	m_ShaderShadow->use();
	glm::mat4 projection = glm::perspective(glm::radians(m_Camera->GetFOV()), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = m_Camera->GetViewMatrix();
	m_ShaderShadow->setMat4("view", view);
	m_ShaderShadow->setMat4("projection", projection);
	m_ShaderShadow->setVec3("viewPos", m_Camera->GetCameraPos());
	m_ShaderShadow->setVec3("lightPos", m_PosLight);
	m_ShaderShadow->setMat4("lightSpaceMatrix", lightSpaceMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_Texture->GetCorrectID());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_DepthMap);
	m_ShaderShadow->setInt("diffuseTexture", 0);
	m_ShaderShadow->setInt("shadowMap", 1);

	m_ShaderShadow->setMat4("model", MatModelFloor);
	m_ModelFloor->Draw();

	MatModelCube = glm::mat4(1.f);
	MatModelCube = glm::translate(MatModelCube, glm::vec3(0.f, 1.5f, 0.f));
	m_ShaderShadow->setMat4("model", MatModelCube);
	m_ModelCube->Draw();

	MatModelCube = glm::mat4(1.f);
	MatModelCube = glm::translate(MatModelCube, glm::vec3(2.f, 0.f, 1.f));
	m_ShaderShadow->setMat4("model", MatModelCube);
	m_ModelCube->Draw();

	MatModelCube = glm::mat4(1.f);
	MatModelCube = glm::translate(MatModelCube, glm::vec3(-1.f, 0.f, 2.f));
	MatModelCube = glm::rotate(MatModelCube, glm::radians(60.f), glm::normalize(glm::vec3(1.f, 0.f, 1.f)));
	m_ShaderShadow->setMat4("model", MatModelCube);
	m_ModelCube->Draw();

	// render Depth map to quad for visual debugging
	/*m_ShaderDebug->use();
	m_ShaderDebug->setFloat("near_plane", nearPlane);
	m_ShaderDebug->setFloat("far_plane", farPlane);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_DepthMap);
	m_ShaderDebug->setInt("depthMap", 0);
	RenderQuad();*/
}

void RenderFeature_ShadowMap::RenderQuad()
{
	if (m_VAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);
		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(m_VAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}