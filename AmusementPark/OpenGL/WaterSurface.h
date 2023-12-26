#pragma once
#include <vector>
#include "GameHeader.h"
#include "BufferObject.h"
#include "Terrain.h"
#include "Shader.h"
#include "Viewer.h"

class Viewer;

class WaterSurface
{
public:
	WaterSurface(Viewer* viewer);

	void Update();
	void Draw(Shader* shader);

	void ResizeFBO();

	FBO* reflectionFBO = nullptr;
	FBO* refractionFBO = nullptr;
private:
	const int VERTEX_COUNT = 512;
	const int SIZE = 10;
	float t = 0;

	Viewer* m_viewer = nullptr;
	std::vector<Texture2D*>* m_heightMaps = nullptr;

	VAO* m_waterVAO = nullptr;
};

