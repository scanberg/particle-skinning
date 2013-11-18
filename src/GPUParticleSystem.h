#pragma once

#include <GL/glew.h>
#include "ParticleSystem.h"
#include "Shader.h"

class GPUParticleSystem : public ParticleSystem
{
public:
	GPUParticleSystem(sParticle * particleData, unsigned int particleCount, Shader& shader);
	~GPUParticleSystem();

	void setStaticAcceleration(const glm::vec3 &acc);
	void setStaticForce(const glm::vec3 &force);

	void addForce(const glm::vec3 &force);
	void addImpulse(const glm::vec3 &impulse);

	//void addRandomForce();
	//void addRandomImpulse();

	void update(float dt);

	unsigned int getSourceVA() { return m_va[!m_target]; }
	unsigned int getTargetVA() { return m_va[m_target]; }

	unsigned int getSourceVB() { return m_vb[!m_target]; }
	unsigned int getTargetVB() { return m_vb[m_target]; }

	unsigned int getParticleCount() { return m_particleCount; }
private:
	void swapTarget(){ m_target = !m_target; }

	glm::vec3 m_staticAcc;
	glm::vec3 m_staticForce;

	unsigned int 	m_va[2];
	unsigned int 	m_vb[2];
	unsigned int 	m_particleCount;

	unsigned int 	m_target;
	Shader *		m_shader;
};