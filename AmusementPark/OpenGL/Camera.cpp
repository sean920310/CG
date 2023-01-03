#include "Camera.h"

int Camera::width = INIT_SCREEN_WIDTH;
int Camera::height = INIT_SCREEN_HEIGHT;
bool Camera::hideMouse = false;


Camera::Camera(Viewer* viewer, glm::vec3 position)
{
	this->position = position;
	this->viewer = viewer;
}

void Camera::Resize(int width, int height)
{
	Camera::width = width;
	Camera::height = height;
}

void Camera::ToggleMouse()
{
	hideMouse = !hideMouse;
	if(!hideMouse)
		firstClick = true;

	//std::cout << "switch mouse!!" << std::endl;
}

void Camera::CommomMatrix()
{
	viewMatrix = glm::lookAt(position, position + orientation, up);
	projectionMatrix = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearZ, farZ);
}

void Camera::Update()
{
	if (hideMouse)
		glfwSetInputMode(viewer->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	else
		glfwSetInputMode(viewer->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	this->KeyInputs();
}

void Camera::KeyInputs()
{
	//key
	if (glfwGetKey(viewer->window, GLFW_KEY_W) == GLFW_PRESS)
	{
		position += speed * orientation;
	}
	if (glfwGetKey(viewer->window, GLFW_KEY_S) == GLFW_PRESS)
	{
		position += -speed * orientation;
	}
	if (glfwGetKey(viewer->window, GLFW_KEY_A) == GLFW_PRESS)
	{
		position += -speed * glm::normalize(glm::cross(orientation, up));
	}
	if (glfwGetKey(viewer->window, GLFW_KEY_D) == GLFW_PRESS)
	{
		position += speed * glm::normalize(glm::cross(orientation, up));
	}
	if (glfwGetKey(viewer->window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		position += speed * up;
	}
	if (glfwGetKey(viewer->window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		position += -speed * up;
	}
	if (glfwGetKey(viewer->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		speed = 0.1;
	}
	else if (glfwGetKey(viewer->window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		speed = 0.05f;
	}
	//if (position.y < 0.1f)position.y = 0.1f;
}


void Camera::MouseInputs(double xpos, double ypos)
{
	if (hideMouse)
	{
		if (firstClick)
		{
			lastX = xpos;
			lastY = ypos;
			firstClick = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;

		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		if (yaw >= 180.0f)
			yaw -= 360.0f;
		if (yaw < -180.0f)
			yaw += 360.0f;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		orientation = glm::normalize(direction);
	}
}
