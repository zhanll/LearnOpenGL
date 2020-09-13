#include "FrameBuffer.h"
#include "../Shader.h"
#include "../Model.h"
#include "../Config.h"
#include <glm/gtc/matrix_transform.hpp>

RenderFeature_FrameBuffer::~RenderFeature_FrameBuffer()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_FBO);
	glDeleteTextures(1, &m_CBO);
	glDeleteBuffers(1, &m_RBO);
}

void RenderFeature_FrameBuffer::Setup()
{
	m_ShaderBasic = std::make_shared<Shader>("res/shaders/basic.vs", "res/shaders/basic.fs");
	m_ShaderScreen = std::make_shared < Shader>("res/shaders/screen.vs", "res/shaders/screen.fs");
	m_Model = std::make_shared<Model>("res/models/backpack/backpack.obj");

	/** Vertex Data */
	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions       // texCoords
		-1.0f,  1.0f,       0.0f, 1.0f,
		-1.0f, -1.0f,       0.0f, 0.0f,
		 1.0f, -1.0f,       1.0f, 0.0f,

		-1.0f,  1.0f,       0.0f, 1.0f,
		 1.0f, -1.0f,       1.0f, 0.0f,
		 1.0f,  1.0f,       1.0f, 1.0f
	};

	// screen quad VAO
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	/** Frame Buffer */
	glGenFramebuffers(1, &m_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	// create a color attachment texture
	glGenTextures(1, &m_CBO);
	glBindTexture(GL_TEXTURE_2D, m_CBO);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_CBO, 0);

	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	glGenRenderbuffers(1, &m_RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO); // now actually attach it

	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderFeature_FrameBuffer::Render()
{
	RenderFeatureBase::Render();

	// bind to framebuffer and draw scene as we normally would to color texture 
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	m_ShaderBasic->use();
	m_ShaderBasic->setMat4("view", m_MatView);
	m_ShaderBasic->setMat4("projection", m_MatProjection);

	/** backpack */
	glm::mat4 backpackMat = glm::mat4(1.0f);
	backpackMat = glm::translate(backpackMat, glm::vec3(0.f, 2.f, -5.f));
	m_ShaderBasic->setMat4("model", backpackMat);

	m_Model->Draw(*m_ShaderBasic);

	// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.

	m_ShaderScreen->use();
	glBindVertexArray(m_VAO);
	glBindTexture(GL_TEXTURE_2D, m_CBO);   // use the color attachment texture as the texture of the quad plane
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
