#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 InPos, float InYaw, float InPitch)
	:cameraPos(InPos), yaw(InYaw), pitch(InPitch)
{
}

Camera::Camera(float posX, float posY, float posZ, float InYaw, float InPitch)
	:cameraPos(posX, posY, posZ), yaw(InYaw), pitch(InPitch)
{
}

Camera::~Camera()
{
}

glm::mat4 Camera::GetViewMatrix() const
{
	return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

void Camera::MoveForward(float deltaTime)
{
	cameraPos += cameraSpeed * deltaTime * cameraFront;
}

void Camera::MoveRight(float deltaTime)
{
	cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * deltaTime;
}

void Camera::LookAround(float xoffset, float yoffset)
{
	yaw += xoffset * mouseSensitivity;
	pitch += yoffset * mouseSensitivity;

	if (pitch > 89.f)
	{
		pitch = 89.f;
	}
	else if (pitch < -89.f)
	{
		pitch = -89.f;
	}

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}

void Camera::Zoom(float offset)
{
	fov -= (float)offset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 90.0f)
		fov = 90.0f;
}
