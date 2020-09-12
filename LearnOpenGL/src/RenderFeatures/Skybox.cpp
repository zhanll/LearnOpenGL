#include "Skybox.h"
#include "../Shader.h"
#include "../Model.h"
#include "../Camera.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>


void RenderFeature_Skybox::Setup()
{
	m_Shader = std::make_shared<Shader>("res/shaders/skybox.vs", "res/shaders/skybox.fs");
	m_Model = std::make_shared<Model>("res/models/cube/cube.obj");
	std::vector<std::string> faces
	{
		"res/textures/skybox/right.jpg",
		"res/textures/skybox/left.jpg",
		"res/textures/skybox/top.jpg",
		"res/textures/skybox/bottom.jpg",
		"res/textures/skybox/front.jpg",
		"res/textures/skybox/back.jpg"
	};
	m_CubemapTexture = LoadCubemap(faces);
}

void RenderFeature_Skybox::Render()
{
	RenderFeatureBase::Render();

	if (!m_Camera)
	{
		return;
	}

	// draw skybox as last
	glCullFace(GL_FRONT);
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content

	m_Shader->use();
	m_Shader->setInt("skybox", 0);

	glm::mat4 skyboxMat = glm::mat4(1.0f);
	skyboxMat = glm::translate(skyboxMat, m_Camera->GetCameraPos());
	skyboxMat = glm::scale(skyboxMat, glm::vec3(2.0f, 2.0f, 2.0f));

	m_Shader->setMat4("model", skyboxMat);
	m_Shader->setMat4("view", m_MatView);
	m_Shader->setMat4("projection", m_MatProjection);

	// skybox cube
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubemapTexture);

	m_Model->Draw(*m_Shader);

	glDepthFunc(GL_LESS); // set depth function back to default
	glCullFace(GL_BACK);
}


// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int RenderFeature_Skybox::LoadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}
