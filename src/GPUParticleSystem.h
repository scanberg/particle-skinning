#pragma once

#include <GL/glew.h>
#include "ParticleSystem.h"
#include "Shader.h"
#include "Body.h"

class GPUParticleSystem : public ParticleSystem
{
public:
	GPUParticleSystem(sParticle * particleData, size_t particleCount, Shader& shader);
	~GPUParticleSystem();

	void setData(sParticle * particleData, size_t particleCount);

	void setStaticAcceleration(const glm::vec3 &acc);
	void setStaticForce(const glm::vec3 &force);

	void addForce(const glm::vec3 &force)		{ m_forceAccumulator += force; }
	void addImpulse(const glm::vec3 &impulse)	{ m_impulseAccumulator += impulse; }

	void update(float dt);

	Shader& 	getShader() { return m_shader; }

	GLuint 		getSourceVA() { return m_va[!m_target]; }
	GLuint 		getTargetVA() { return m_va[m_target]; }

	GLuint 		getSourceVB() { return m_vb[!m_target]; }
	GLuint 		getTargetVB() { return m_vb[m_target]; }

	size_t 		getParticleCount() { return m_particleCount; }

	void swapTarget(){ m_target = !m_target; }

private:
	glm::vec3	m_staticAcceleration;
	glm::vec3	m_staticForce;

	glm::vec3	m_forceAccumulator;
	glm::vec3	m_impulseAccumulator;

	float		m_time;

	GLuint 		m_va[2];
	GLuint 		m_vb[2];
	size_t 		m_particleCount;

	int 		m_target;

	Shader&		m_shader;
};