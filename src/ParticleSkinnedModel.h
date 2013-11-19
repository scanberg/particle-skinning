#pragma once

#include "GPUParticleSystem.h"
#include "Model.h"
#include "Shader.h"

class ParticleSkinnedModel : public Model
{
public:
	ParticleSkinnedModel(Shader * particleShader, Body * body, Material ** mat = NULL, unsigned int materialCount = 0);
	~ParticleSkinnedModel();

	void addRandomImpulse(float impulse) { m_ps.addRandomImpulse(impulse); }

	void update(float dt);
	void draw();

protected:

	GPUParticleSystem m_ps;
};