#pragma once

#include <GL/glew.h>
#include "ParticleSystem.h"
#include "Shader.h"
#include "Body.h"

class GPUParticleSystem : public ParticleSystem
{
public:
	GPUParticleSystem(sParticle * particleData, unsigned int particleCount, Shader * shader);
	~GPUParticleSystem();

	void setStaticAcceleration(const glm::vec3 &acc);
	void setStaticForce(const glm::vec3 &force);

	void addForce(const glm::vec3 &force)		{ m_forceAccumulator += force; }
	void addImpulse(const glm::vec3 &impulse)	{ m_impulseAccumulator += impulse; }

	//void addRandomForce(float min, float max);
	void addRandomImpulse(float impulse);

	void update(float dt);

	Shader * 		getShader() { return m_shader; }

	unsigned int 	getSourceVA() { return m_va[!m_target]; }
	unsigned int 	getTargetVA() { return m_va[m_target]; }

	unsigned int 	getSourceVB() { return m_vb[!m_target]; }
	unsigned int 	getTargetVB() { return m_vb[m_target]; }

	unsigned int 	getParticleCount() { return m_particleCount; }
private:
	void initData(sParticle * particleData, unsigned int particleCount);
	void swapTarget(){ m_target = !m_target; }

	glm::vec3	m_staticAcceleration;
	glm::vec3	m_staticForce;

	glm::vec3	m_forceAccumulator;
	glm::vec3	m_impulseAccumulator;

	float		m_randomForceAccumulator;

	float			m_time;

	unsigned int 	m_va[2];
	unsigned int 	m_vb[2];
	unsigned int 	m_particleCount;

	unsigned int 	m_target;
	Shader *		m_shader;
};