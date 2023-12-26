#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "GameHeader.h"
#include "Shader.h"
#include "Viewer.h"

class Viewer;

class Camera
{
public:
	Viewer* viewer;

	glm::vec3 position;
	glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;

	float speed = 0.1f;
	float sensitivity = 0.1f;

	float yaw = -90.0f;
	float pitch = 0.0f;
	float lastX , lastY;

	static int width;
	static int height;

	static bool hideMouse;

public:
	Camera(Viewer* viewer, glm::vec3 position);

	static void Resize(int width, int height);

	void CommomMatrix();
	void ToggleMouse();
	void Update();
	void KeyInputs();
	void MouseInputs(double xpos, double ypos);
	
	void invertPitch();

private:
	bool firstClick = true;

	const float FOVdeg = 90.0f;
	const float nearZ = 0.01f;
	const float farZ = 1000.0f;

};

