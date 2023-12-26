#pragma once
#include <vector>
#include <fstream>

#include "GameHeader.h"
#include "BufferObject.h"
#include "Texture.h"
#include "Shader.h"


class Terrain
{
public:
	Terrain(int gridX, int gridZ, const std::string& texturePath, const std::string& heightMapPath);
	~Terrain();

	void Draw(Shader* shader);

private:
	const int SIZE = 200;
	const int VERTEX_COUNT = 256;
	const float HEIGHT = 10.0;

	float x, z;

	VAO* m_vao = nullptr;
	Texture2D* m_texture = nullptr;
	Texture2D* m_heightMap = nullptr;

};

