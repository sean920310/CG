#pragma once
#include "GameHeader.h"

class DirLight
{
public:
	DirLight(glm::vec4 position,glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular)
		: direction(position), ambient(ambient), diffuse(diffuse), specular(specular) {}

public:

	glm::vec4 direction;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
};

class PosLight
{
public:
	PosLight(glm::vec4 position, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular, float constant, float linear, float quadratic)
		: position(position), ambient(ambient), diffuse(diffuse), specular(specular), constant(constant), linear(linear), quadratic(quadratic) {}

public:

	glm::vec4 position;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;

	float constant;
	float linear;
	float quadratic;
};

class SpotLight
{
public:
	SpotLight(glm::vec4 position, glm::vec4 direction, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular, float constant, float linear, float quadratic,float cutOff,float outerCutOff) 
		: position(position), direction(direction), ambient(ambient), diffuse(diffuse), specular(specular), constant(constant), linear(linear), quadratic(quadratic), cutOff(cutOff), outerCutOff(outerCutOff) {}

public:

	glm::vec4 position;
	glm::vec4 direction;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;

	float constant;
	float linear;
	float quadratic;

	float cutOff;
	float outerCutOff;
};