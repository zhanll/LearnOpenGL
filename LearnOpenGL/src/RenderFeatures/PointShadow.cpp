#include "PointShadow.h"
#include "glad/glad.h"
#include "../Config.h"
#include "../Shader.h"
#include "../Model.h"
#include "../Camera.h"
#include "../Texture.h"
#include "GLFW/glfw3.h"
#include <glm/gtc/matrix_transform.hpp>

const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

void RenderFeature_PointShadow::Setup()
{
	m_shaderDepth = std::make_shared<Shader>("res/shaders/point_shadow_depth.vs", "res/shaders/point_shadow_depth.fs", "res/shaders/point_shadow_depth.gs");
	m_shaderScene = std::make_shared<Shader>("res/shaders/point_shadow.vs", "res/shaders/point_shadow.fs");
	m_shaderLight = std::make_shared<Shader>("res/shaders/basic.vs", "res/shaders/light_cube.fs");
	m_shaderDebug = std::make_shared<Shader>("res/shaders/skybox.vs", "res/shaders/skybox.fs");
	m_modelContainer = std::make_shared<Model>("res/models/container/cube.obj");
	m_modelFloor = std::make_shared<Model>("res/models/plane/plane.obj");
	m_modelLight = std::make_shared<Model>("res/models/cube/cube.obj");
	m_Texture = std::make_shared<Texture>("texture_diffuse", "res/textures/wall.jpg");
	m_Texture->LoadFromFile();

	m_lightPos = glm::vec3(0.f, 0.f, 0.f);

	glGenFramebuffers(1, &m_depthMapFBO);
	glGenTextures(1, &m_depthCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthCubemap);
	for (unsigned int i=0; i<6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthCubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_shaderScene->use();
	m_shaderScene->setInt("diffuseTexture", 0);
	m_shaderScene->setInt("depthMap", 1);
}

void RenderFeature_PointShadow::Render()
{
	RenderFeatureBase::Render();

	m_lightPos.z = std::sin((float)glfwGetTime() * 0.5f) * 3.0f;

	// 0. create depth cubemap transformation matrices
	float near_plane = 1.0f;
	float far_plane = 25.f;
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.f), (float)SHADOW_WIDTH / SHADOW_HEIGHT, near_plane, far_plane);
	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(shadowProj * glm::lookAt(m_lightPos, m_lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(m_lightPos, m_lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(m_lightPos, m_lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(m_lightPos, m_lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(m_lightPos, m_lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(m_lightPos, m_lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
																																																		
	// 1. first render to depth map
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	m_shaderDepth->use();
	for (unsigned int i=0; i<6; ++i)
	{
		m_shaderDepth->setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
	}
	m_shaderDepth->setFloat("far_plane", far_plane);
	m_shaderDepth->setVec3("lightPos", m_lightPos);
	RenderScene(m_shaderDepth);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 2. then render scene as normal with shadow mapping (using depth cubemap)
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_shaderScene->use();
	m_shaderScene->setMat4("projection", m_MatProjection);
	m_shaderScene->setMat4("view", m_MatView);
	m_shaderScene->setVec3("lightPos", m_lightPos);
	m_shaderScene->setVec3("viewPos", m_Camera->GetCameraPos());
	m_shaderScene->setFloat("far_plane", far_plane);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_Texture->GetCorrectID());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthCubemap);
	RenderScene(m_shaderScene);

	// floor
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.f, -4.0f, 0.f));
	model = glm::scale(model, glm::vec3(50.f));
	m_shaderScene->setMat4("model", model);
	m_modelFloor->Draw(*m_shaderScene);

	// light
	model = glm::mat4(1.0f);
	model = glm::translate(model, m_lightPos);
	model = glm::scale(model, glm::vec3(0.1f));
	m_shaderLight->use();
	m_shaderLight->setMat4("projection", m_MatProjection);
	m_shaderLight->setMat4("view", m_MatView);
	m_shaderLight->setMat4("model", model);
	m_modelLight->Draw(*m_shaderLight);

	// render Depth map to quad for visual debugging
	/*glCullFace(GL_FRONT);
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content

	m_shaderDebug->use();
	m_shaderDebug->setInt("skybox", 0);

	glm::mat4 skyboxMat = glm::mat4(1.0f);
	skyboxMat = glm::translate(skyboxMat, m_Camera->GetCameraPos());
	skyboxMat = glm::scale(skyboxMat, glm::vec3(2.0f, 2.0f, 2.0f));

	m_shaderDebug->setMat4("model", skyboxMat);
	m_shaderDebug->setMat4("view", m_MatView);
	m_shaderDebug->setMat4("projection", m_MatProjection);

	// skybox cube
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthCubemap);

	m_modelLight->Draw(*m_shaderDebug);

	glDepthFunc(GL_LESS); // set depth function back to default
	glCullFace(GL_BACK);*/
}

void RenderFeature_PointShadow::RenderScene(std::shared_ptr<Shader> InShader)
{
	// cubes
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(4.0f, -3.0f, 0.0));
	model = glm::scale(model, glm::vec3(0.5f));
	InShader->setMat4("model", model);
	m_modelContainer->Draw(*InShader);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, -2.0f, 1.0));
	model = glm::scale(model, glm::vec3(0.75f));
	InShader->setMat4("model", model);
	m_modelContainer->Draw(*InShader);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-3.0f, -1.0f, 0.0));
	model = glm::scale(model, glm::vec3(0.5f));
	InShader->setMat4("model", model);
	m_modelContainer->Draw(*InShader);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.5f, -2.0f, 1.5));
	model = glm::scale(model, glm::vec3(0.5f));
	InShader->setMat4("model", model);
	m_modelContainer->Draw(*InShader);
}
