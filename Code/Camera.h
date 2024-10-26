#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};
float YAW = -90.0f;
float PITCH = 0.0f;
float SENSITIVITY = 0.05f;
float ZOOM = 45.0f;
float SPEED = 2.5f;


class Camera
{
	
public:
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	glm::vec3 Position;
	float Pitch;
	float Yaw;
	float MouseSensitivity;
	float MouseSpeed;
	float Zoom;

	Camera(glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f), float pitch = PITCH, float yaw = YAW) :Front(glm::vec3(0.0f, 0.0f, -1.0f)), WorldUp(glm::vec3(0.0f, 1.0f, 0.0f)), MouseSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM){
		Position = cameraPos;
		WorldUp = glm::normalize(worldUp);
		Pitch = pitch;
		Yaw = yaw;
		updateCameraVectors();

	}
	Camera(float posX = 0.0f, float posY = 0.0f, float posZ = 3.0f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f, float pitch = PITCH, float yaw = YAW) :Front(glm::vec3(0.0f, 0.0f, -1.0f)), WorldUp(glm::vec3(0.0f, 1.0f, 0.0f)), MouseSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::normalize(glm::vec3(upX, upY, upZ));
		Pitch = pitch;
		Yaw = yaw;
		updateCameraVectors();
	}

	glm::mat4 GetViewMatrix() {
		return glm::lookAt(Position, Position + Front, Up);
	}
	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		float velocity = MouseSpeed * deltaTime;
		if (direction == FORWARD)
			Position += Front * velocity;
		if (direction == BACKWARD)
			Position -= Front * velocity;
		if (direction == LEFT)
			Position -= Right * velocity;
		if (direction == RIGHT)
			Position += Right * velocity;
	}

	void ProcessMouseMovement(float xoffset, float yoffset, bool angleConstraint) {
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;
		Pitch += yoffset;
		Yaw += xoffset;
		
		if (angleConstraint) {
			Pitch = std::max(-89.f, std::min(89.f, Pitch));
		}
		updateCameraVectors();
	}

	void ProcessMouseScroll(float yoffset) {
		Zoom -= yoffset;
		Zoom = std::max(0.0f, std::min(45.0f, Zoom));

	}


private:
	
	void updateCameraVectors() {
		glm::vec3 front;
		front.x = cos(glm::radians(Pitch)) * cos(glm::radians(Yaw));
		front.y = sin(glm::radians(Pitch));
		front.z = cos(glm::radians(Pitch)) * sin(glm::radians(Yaw));
		Front = glm::normalize(front);
		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));

		
	}
};
