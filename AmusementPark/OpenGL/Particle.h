#pragma once
#include "GameHeader.h"
#include "BufferObject.h"
#include "Shader.h"

class Particle
{
public:
	Particle(glm::vec3 position, glm::vec3 velocity, float lifeTime, glm::vec3 scale = glm::vec3(1.0f));
	~Particle();

	bool Update(float t);
	void Draw(Shader* shader);

public:

	glm::vec3 color = glm::vec3(1.0f);
	glm::vec3 position;
	glm::vec3 velocity;
	float gravityEffect = 50.0f;
	float lifeTime;
	float rotation;
	glm::vec3 scale;
private:
	float m_elapsedTime = 0.0f;
	VAO* m_vao;
};

