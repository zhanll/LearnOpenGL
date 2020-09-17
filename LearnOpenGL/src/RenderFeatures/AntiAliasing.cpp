#include "AntiAliasing.h"
#include "../Shader.h"
#include "../Model.h"
#include "../Camera.h"
#include "../Config.h"
#include <glm/gtc/matrix_transform.hpp>

void RenderFeature_AntiAliasing::Setup()
{
	m_ShaderCube = std::make_shared<Shader>("res/shaders/basic.vs", "res/shaders/light_cube.fs");
	m_ModelCube = std::make_shared<Model>("res/models/cube/cube.obj");

	// configure MSAA framebuffer
	// --------------------------
	glGenFramebuffers(1, &m_FBOMultiSampled);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBOMultiSampled);

	// create a multisampled color attachment texture
	glGenTextures(1, &m_CBOMultiSampled);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_CBOMultiSampled);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_CBOMultiSampled, 0);

	// create a (also multisampled) renderbuffer object for depth and stencil attachments
	glGenRenderbuffers(1, &m_RBOMultiSampled);
	glBindRenderbuffer(GL_RENDERBUFFER, m_RBOMultiSampled);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBOMultiSampled);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderFeature_AntiAliasing::Render()
{
	RenderFeatureBase::Render();

	// 1. draw scene as normal in multisampled buffers
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBOMultiSampled);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// set transformation matrices		
	m_ShaderCube->use();
	m_ShaderCube->setMat4("projection", m_MatProjection);
	m_ShaderCube->setMat4("view", m_MatView);
	glm::mat4 model(1.0f);
	model = glm::translate(model, glm::vec3(0.f, 3.f, 0.f));
	m_ShaderCube->setMat4("model", model);

	m_ModelCube->Draw(*m_ShaderCube);

	// 2. now blit multisampled buffer(s) to normal colorbuffer of intermediate FBO. Image is stored in screenTexture
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FBOMultiSampled);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}
