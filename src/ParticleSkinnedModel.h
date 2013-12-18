#pragma once

#include <set>

#include "GPUParticleSystem.h"
#include "Model.h"
#include "Shader.h"

class ParticleSkinnedModel : public Model
{
public:
	ParticleSkinnedModel(Shader & particleShader, Body * body, Material ** mat = NULL, unsigned int materialCount = 0);
	~ParticleSkinnedModel();

	void resetParticlePositions();

	void update(float dt);
	virtual void draw();
	virtual void drawPart(size_t index);

	void simulateOnGPU(bool val) { m_simulateOnGPU = val; resetParticlePositions(); }

protected:
	// Pointer, to enable initialization in constructor rather than in the initializer list.
	GPUParticleSystem*	m_ps;
	bool				m_simulateOnGPU;
	glm::vec3			getParticleMass_K_D(size_t index);
	float				distanceToBone(size_t index);

	VertexBuffer					m_particleBuffer;
	std::vector<sParticle>			m_particles;
	std::vector<Body::sLine>		m_constraints;
};
