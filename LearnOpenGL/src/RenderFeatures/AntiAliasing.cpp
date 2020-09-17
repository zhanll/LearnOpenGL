#include "AntiAliasing.h"
#include "../Shader.h"
#include "../Model.h"
#include "../Camera.h"
#include "../Config.h"
#include <glm/gtc/matrix_transform.hpp>

void RenderFeature_AntiAliasing::Setup()
{
	m_ShaderCube = std::make_shared<Shader>("res/shaders/basic.vs", "res/shaders/light_cube.fs");
	m_ShaderScreen = std::make_shared<Shader>("res/shaders/screen.vs", "res/shaders/screen.fs");
	m_ModelCube = std::make_shared<Model>("res/models/cube/cube.obj");

	float quadVertices[] = {   // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	// setup screen VAO
	glGenVertexArrays(1, &m_VAOQuad);
	glGenBuffers(1, &m_VBOQuad);
	glBindVertexArray(m_VAOQuad);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOQuad);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


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

	// configure second post-processing framebuffer
	glGenFramebuffers(1, &m_FBOScreen);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBOScreen);

	// create a color attachment texture
	glGenTextures(1, &m_CBOScreen);
	glBindTexture(GL_TEXTURE_2D, m_CBOScreen);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_CBOScreen, 0);	// we only need a color buffer

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_ShaderScreen->use();
	m_ShaderScreen->setInt("screenTexture", 0);
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
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FBOScreen);
	glBlitFramebuffer(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	// 3. now render quad with scene's visuals as its texture image
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	// draw Screen quad
	m_ShaderScreen->use();
	glBindVertexArray(m_VAOQuad);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_CBOScreen); // use the now resolved color attachment as the quad's texture
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
