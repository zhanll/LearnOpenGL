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

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

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
    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

    /* Enable */
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

    /* Shader */
    Shader basicShader("res/shaders/basic.vs", "res/shaders/basic.fs");
    Shader lightShader("res/shaders/basic.vs", "res/shaders/light.fs");
    Shader outlineShader("res/shaders/basic.vs", "res/shaders/outline.fs");
    Shader transparentShader("res/shaders/basic.vs", "res/shaders/transparent.fs");

    /* Model */
    Model backpackModel("res/models/backpack/backpack.obj");
    Model floorModel("res/models/plane/plane.obj");

    /* Vertex Input */
    float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,    0.0f, 0.0f,  
	 0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  1.0f, 0.0f,  
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   1.0f, 1.0f,   
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   1.0f, 1.0f,   
	-0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  0.0f, 1.0f,  
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,    0.0f, 0.0f,  

	-0.5f, -0.5f,  0.5f,   0.0f,  0.0f, 1.0f,   0.0f, 0.0f,  
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,    1.0f, 0.0f,   
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,    1.0f, 1.0f,   
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,    1.0f, 1.0f,   
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,    0.0f, 1.0f,   
	-0.5f, -0.5f,  0.5f,   0.0f,  0.0f, 1.0f,   0.0f, 0.0f,  

	-0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,   
	-0.5f,  0.5f, -0.5f,   -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,   

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,   1.0f, 0.0f,   
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,   1.0f, 1.0f,   
	 0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  
	 0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,   0.0f, 0.0f,   
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,   1.0f, 0.0f,   

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,   0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,   0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   0.0f,  1.0f,  0.0f
    };
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  4.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -25.0f)
	};
	float planeVertices[] = {
		// positions           // normal           // texture Coords 
		 5.0f, -0.5f,  5.0f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,

		 5.0f, -0.5f,  5.0f,   0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 5.0f, -0.5f, -5.0f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f
	};
	float transparentVertices[] = {
		// positions            // normal                // texture Coords (swapped y coordinates because texture is flipped upside down)
		0.0f,  0.5f,  0.0f,     0.0f,  0.0f,  1.0f,      0.0f,  0.0f,
		0.0f, -0.5f,  0.0f,    0.0f,  0.0f,  1.0f,      0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,    0.0f,  0.0f,  1.0f,      1.0f,  1.0f,

		0.0f,  0.5f,  0.0f,     0.0f,  0.0f,  1.0f,     0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,    0.0f,  0.0f,  1.0f,     1.0f,  1.0f,
		1.0f,  0.5f,  0.0f,     0.0f,  0.0f,  1.0f,     1.0f,  0.0f
	};
	std::vector<glm::vec3> vegetation
	{
		glm::vec3(-1.5f, 0.0f, -0.48f),
		glm::vec3(1.5f, 0.0f, 0.51f),
		glm::vec3(0.0f, 0.0f, 0.7f),
		glm::vec3(-0.3f, 0.0f, -2.3f),
		glm::vec3(0.5f, 0.0f, -0.6f)
	};

    /* Light Cube */
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

    /** Floor Plane */
    unsigned int planeVBO;
    glGenBuffers(1, &planeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

    unsigned int planeVAO;
    glGenVertexArrays(1, &planeVAO);
    glBindVertexArray(planeVAO);
    
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

    /** Grass */
    unsigned int grassVBO;
    glGenBuffers(1, &grassVBO);
    glBindBuffer(GL_ARRAY_BUFFER, grassVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);

    unsigned int grassVAO;
    glGenVertexArrays(1, &grassVAO);
    glBindVertexArray(grassVAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

    /* Unbind */
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // render loop
    while (!glfwWindowShouldClose(window))
    {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

        // input
        processInput(window);

        // rendering commands
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// 1st. render pass, draw objects as normal, writing to the stencil buffer
		// --------------------------------------------------------------------
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);  // all fragments should pass the stencil test
        glStencilMask(0xFF);    // enable writing to the stencil buffer

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

        glm::mat4 backpackMat = glm::mat4(1.0f);
        backpackMat = glm::translate(backpackMat, glm::vec3(0.f, 2.f, -5.f));
        basicShader.setMat4("model", backpackMat);

        backpackModel.Draw(basicShader);

        glm::mat4 floorMat = glm::mat4(1.0f);
        floorMat = glm::translate(floorMat, glm::vec3(0.f, 0.f, 0.f));
        floorMat = glm::scale(floorMat, glm::vec3(50.f, 50.f, 50.f));
        basicShader.setMat4("model", floorMat);

        floorModel.Draw(basicShader);

		// 2nd. render pass: now draw slightly scaled versions of the objects, this time disabling stencil writing.
		// Because the stencil buffer is now filled with several 1s. The parts of the buffer that are 1 are not drawn, thus only drawing 
		// the objects' size differences, making it look like borders.
		// -----------------------------------------------------------------------------------------------------------------------------
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);    // disable writing to the stencil buffer
        glDisable(GL_DEPTH_TEST);

        outlineShader.use();
        outlineShader.setMat4("view", view);
        outlineShader.setMat4("projection", projection);

        glm::mat4 scaledMeshModel = glm::mat4(1.0f);
        scaledMeshModel = glm::translate(scaledMeshModel, glm::vec3(0.f, 2.f, -5.f));
        scaledMeshModel = glm::scale(scaledMeshModel, glm::vec3(1.01f, 1.01f, 1.01f));
        outlineShader.setMat4("model", scaledMeshModel);

        backpackModel.Draw(outlineShader);

        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glEnable(GL_DEPTH_TEST);

        /* light */
        lightShader.use();
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);

        glBindVertexArray(lightVAO);
		for (unsigned int i = 0; i < 1; i++)
		{
			glm::mat4 lightModel = glm::mat4(1.0f);
			lightModel = glm::scale(lightModel, glm::vec3(0.2f));
			lightModel = glm::translate(lightModel, pointLightPositions[i]);

			lightShader.setMat4("model", lightModel);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

        // check all events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    /* Clean up */
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();

    return 0;
}