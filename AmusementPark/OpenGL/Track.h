#pragma once
#include <vector>
#include "GameHeader.h"

class ControlPoint
{
public:
	ControlPoint();
	ControlPoint(const glm::vec3& pos);
	ControlPoint(const glm::vec3& pos, const glm::vec3& orient);

public:
	glm::vec3 pos;
	glm::vec3 orient;
};

class Track
{
public:
	Track();

	void readPoints(const char* filename);
public:
	std::vector<ControlPoint> points;

	float trainU;
	int carCount;
	std::vector<float> arcLength;
	std::vector<std::vector<float>> length;
};

