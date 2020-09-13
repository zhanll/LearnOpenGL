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
#include "RenderFeatures/Blend.h"
#include "RenderFeatures/Light.h"
#include "RenderFeatures/FrameBuffer.h"


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
        {"Skybox", std::make_shared<RenderFeature_Skybox>(&camera)},
        {"Blend", std::make_shared<RenderFeature_Blend>(&camera)},
        {"Light", std::make_shared<RenderFeature_Light>(&camera)},
        {"FrameBuffer", std::make_shared<RenderFeature_FrameBuffer>(&camera)}
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

        // rendering commands
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        if (SelectedRenderFeature)
        {
            SelectedRenderFeature->Render();
        }

        // check all events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}