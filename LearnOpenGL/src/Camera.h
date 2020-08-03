#pragma once
#include <glm/glm.hpp>

class Camera
{
public:
	Camera(glm::vec3 InPos, float InYaw, float InPitch);
	Camera(float posX, float posY, float posZ, float InYaw, float InPitch);
	virtual ~Camera();

	glm::mat4 GetViewMatrix() const;

	inline float GetFOV() const { return fov; }

	void MoveForward(float deltaTime);
	void MoveRight(float deltaTime);

	void LookAround(float xoffset, float yoffset);

	void Zoom(float offset);

protected:
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	float cameraSpeed = 2.5f;
	float mouseSensitivity = 0.1f;
	float yaw = -90.f;
	float pitch = 0.f;
	float fov = 45.f;
};