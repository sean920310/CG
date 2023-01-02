#pragma once
#include <vector>

#include "GameHeader.h"
#include "BufferObject.h"
#include "Texture.h"
#include "Shader.h"


class Terrain
{
public:
	Terrain(int gridX, int gridZ, const std::string& texturePath);
	~Terrain();

	void Draw(Shader* shader);

private:
	const int SIZE = 800;
	const int VERTEX_COUNT = 128;

	float x, z;

	VAO* m_vao = nullptr;
	Texture2D* m_texture = nullptr;
};

