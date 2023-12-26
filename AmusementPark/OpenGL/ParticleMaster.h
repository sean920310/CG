#pragma once
#include <vector>

#include "GameHeader.h"
#include "Shader.h"
#include "Particle.h"

class ParticleMaster
{
public:
	ParticleMaster();

	void Update(float t);
	void Draw(Shader* shader);
	void AddParticle(Particle* particle);
private:
	std::vector<Particle*> m_particles;
};

