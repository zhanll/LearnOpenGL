#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Config.h"
#include "RenderFeatures/RenderFeature.h"
#include "RenderFeatures/GeometryShader.h"
#include "RenderFeatures/Skybox.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

Camera camera(glm::vec3(0.f, 3.f, 3.f), -90.f, 0.f);

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.MoveForward(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.MoveForward(-deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.MoveRight(-deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.MoveRight(deltaTime);
}

float lastX = SCREEN_WIDTH / 2.f;
float lastY = SCREEN_HEIGHT / 2.f;
bool firstMouse = true;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
    lastX = xpos;
    lastY = ypos;

    camera.LookAround(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.Zoom(yoffset);
}

int main()
{
	/* Select Render Feature */
	struct RenderFeatureUnit
	{
		std::string Name;
		std::shared_ptr<RenderFeatureBase> RenderFeature;
	};
	std::vector<RenderFeatureUnit> RenderFeatures =
	{
		{"GeometryShader", std::make_shared<RenderFeature_GeometryShader>(&camera)},
        {"Skybox", std::make_shared<RenderFeature_Skybox>(&camera)}
	};
	std::shared_ptr<RenderFeatureBase> SelectedRenderFeature = nullptr;
	std::cout << "Select Render Feature:" << std::endl;
	std::cout << "-------------------------" << std::endl;
	for (size_t i = 0; i < RenderFeatures.size(); i++)
	{
		std::cout << i + 1 << "\t" << RenderFeatures[i].Name << std::endl;
	}
	std::cout << "-------------------------" << std::endl;
	int n = 0;
	std::cin >> n;
	if (n > 0 && n <= RenderFeatures.size())
	{
		SelectedRenderFeature = RenderFeatures[n - 1].RenderFeature;
	}

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // for Mac OS X

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    /* Mouse */
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    /* Query max vertex attributes supported */
    /*int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;*/

    /* Enable */
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_CULL_FACE);

    /* Shader */
    Shader basicShader("res/shaders/basic.vs", "res/shaders/basic.fs");
    Shader lightShader("res/shaders/basic.vs", "res/shaders/light.fs");
    Shader outlineShader("res/shaders/basic.vs", "res/shaders/outline.fs");
    Shader transparentShader("res/shaders/basic.vs", "res/shaders/transparent.fs");
    Shader screenShader("res/shaders/screen.vs", "res/shaders/screen.fs");

    /* Model */
    Model backpackModel("res/models/backpack/backpack.obj");
    Model floorModel("res/models/plane/plane.obj");
    Model cubeModel("res/models/cube/cube.obj");
    Model grassModel("res/models/grass/plane.obj");

    /* Position Input */
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  4.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -25.0f)
	};
	std::vector<glm::vec3> vegetation
	{
		glm::vec3(-1.5f, 0.0f, -0.48f),
		glm::vec3(1.5f, 0.0f, 0.51f),
		glm::vec3(0.0f, 0.0f, 0.7f),
		glm::vec3(-0.3f, 0.0f, -2.3f),
		glm::vec3(0.5f, 0.0f, -0.6f)
	};

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
	unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    /** Frame Buffer */
	/*unsigned int fbo;
	glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// create a color attachment texture
	unsigned int textureColorbuffer;
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it

	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);*/

    if (SelectedRenderFeature)
    {
        SelectedRenderFeature->Setup();
    }

    // render loop
    while (!glfwWindowShouldClose(window))
    {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

        // input
        processInput(window);

		// bind to framebuffer and draw scene as we normally would to color texture 
		//glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		//glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

        // rendering commands
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        /* basic */
        basicShader.use();
        basicShader.setFloat("material.shininess", 32.0f);

        basicShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        basicShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        basicShader.setVec3("dirLight.diffuse", 0.3f, 0.3f, 0.3f);
        basicShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

		basicShader.setVec3("pointLight[0].position", pointLightPositions[0]);
		basicShader.setVec3("pointLight[0].ambient", 0.05f, 0.05f, 0.05f);
		basicShader.setVec3("pointLight[0].diffuse", 0.5f, 0.5f, 0.5f);
		basicShader.setVec3("pointLight[0].specular", 1.0f, 1.0f, 1.0f);
		basicShader.setFloat("pointLight[0].constant", 1.0f);
		basicShader.setFloat("pointLight[0].linear", 0.09f);
		basicShader.setFloat("pointLight[0].quadratic", 0.032f);

        basicShader.setVec3("spotLight.position", camera.GetCameraPos());
        basicShader.setVec3("spotLight.direction", camera.GetCameraDirection());
		basicShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		basicShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		basicShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        basicShader.setFloat("spotLight.constant", 1.0f);
        basicShader.setFloat("spotLight.linear", 0.09f);
        basicShader.setFloat("spotLight.quadratic", 0.032f);
        basicShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        basicShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));

        basicShader.setVec3("viewPos", camera.GetCameraPos());

		glm::mat4 view;
        view = camera.GetViewMatrix();

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(camera.GetFOV()), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

        basicShader.setMat4("view", view);
        basicShader.setMat4("projection", projection);

        /** backpack */
        /*glm::mat4 backpackMat = glm::mat4(1.0f);
        backpackMat = glm::translate(backpackMat, glm::vec3(0.f, 2.f, -5.f));
        basicShader.setMat4("model", backpackMat);

        backpackModel.Draw(basicShader);*/

        /** floor */
        glm::mat4 floorMat = glm::mat4(1.0f);
        floorMat = glm::translate(floorMat, glm::vec3(0.f, -1.5f, 0.f));
        floorMat = glm::scale(floorMat, glm::vec3(50.f, 50.f, 50.f));
        basicShader.setMat4("model", floorMat);

        floorModel.Draw(basicShader);

        /* light cube */
        lightShader.use();
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);

        for (unsigned int i = 0; i < 1; i++)
        {
            glm::mat4 lightModel = glm::mat4(1.0f);
            lightModel = glm::scale(lightModel, glm::vec3(0.2f));
            lightModel = glm::translate(lightModel, pointLightPositions[i]);

            lightShader.setMat4("model", lightModel);

            cubeModel.Draw(lightShader);
        }

        /** grass */
        transparentShader.use();
        transparentShader.setMat4("view", view);
        transparentShader.setMat4("projection", projection);

        for (size_t i = 0; i < vegetation.size(); i++)
        {
            glm::mat4 grassMat = glm::mat4(1.0f);
            grassMat = glm::translate(grassMat, vegetation[i]);
            grassMat = glm::scale(grassMat, glm::vec3(3.0f));
            grassMat = glm::rotate(grassMat, glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));

            transparentShader.setMat4("model", grassMat);

            grassModel.Draw(transparentShader);
        }

        if (SelectedRenderFeature)
        {
            SelectedRenderFeature->Render();
        }

		// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
		/*glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.

        screenShader.use();
        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);   // use the color attachment texture as the texture of the quad plane
        glDrawArrays(GL_TRIANGLES, 0, 6);*/

        // check all events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);

    glfwTerminate();

    return 0;
}