#include "ParticleMaster.h"

ParticleMaster::ParticleMaster()
{
}

void ParticleMaster::Update(float t)
{
	for (int i = m_particles.size() - 1; i >= 0; i--)
	{
		if (m_particles[i]->Update(t))
		{
			Particle* temp = m_particles[i];
			m_particles.erase(m_particles.begin() + i);
			delete temp;
		}
	}
}

void ParticleMaster::Draw(Shader* shader)
{
	for (Particle* particle : m_particles)
	{
		particle->Draw(shader);
	}
}

void ParticleMaster::AddParticle(Particle* particle)
{
	m_particles.push_back(particle);
}
