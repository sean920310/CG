#pragma once
#include <vector>
#include "GameHeader.h"
#include "Shader.h"
#include "BufferObject.h"

class ControlPoint
{
public:
	ControlPoint(const glm::vec3& pos, const glm::vec3& orient): pos(pos)
	{
		this->orient = glm::normalize(orient);
	}

public:
	glm::vec3 pos;
	glm::vec3 orient;
};

class Track
{
public:
	Track(const char* filename);

	void ReadPoints(const char* filename);
	void Init();
	void Draw(Shader* shader);


	float AddArcLen(float trainU ,float len);
	static glm::vec3 cubicSpline(glm::vec3 g[4], float t);
public:
	std::vector<ControlPoint> points;

	std::vector<float> arcLength;
	std::vector<std::vector<float>> length;

	VAO* trackVAO = nullptr;
	VAO* sleeperVAO = nullptr;
	Shader* shader = nullptr;

private:
	float sleeperSpace = 8.0f;
	unsigned int trackVerticeCount;
	unsigned int sleeperVerticeCount;

};

