#pragma once
#include "GameHeader.h"
#include "Camera.h"
#include "BufferObject.h"
#include "Texture.h"
#include "Model.h"
#include "Terrain.h"

void frambufferCallback(GLFWwindow* window, int width, int height);
void keyInputCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);

class Camera;

class Viewer
{
public:
	Viewer(int width, int height, const char* name);
	~Viewer();

	void Init();
	void DrawShadowMap();
	void DrawEntity();
	void DrawImGui();
	void Update();

	void frambufferCallbackHandler(int width, int height);
	void keyInputCallbackHandler(int key, int scancode, int action, int mode);
	void mouseCallbackHandler(double xpos, double ypos);
public:
	GLFWwindow* window = nullptr;
	Camera* camera = nullptr;
	int width, height;

private:
	glm::mat4 lightSpaceMatrix;

	VAO* vao = nullptr;
	Texture2D* texture = nullptr;
	Shader* shader = nullptr;

	VAO* skybox = nullptr;
	Texture3D* skyboxTex = nullptr;
	Shader* skyboxShader = nullptr;

	VAO* cube = nullptr;
	float cubeShininess = 1.0f;

	DirLight* dirLight = nullptr;
	SpotLight* spotLight = nullptr;

	std::vector<Terrain*> terrains;
	Shader* terrainShader = nullptr;

	FBO* depthMapFBO = nullptr;
	Shader* depthMapShader = nullptr;

	VAO* debugVAO = nullptr;
	Shader* debugShader = nullptr;
};

