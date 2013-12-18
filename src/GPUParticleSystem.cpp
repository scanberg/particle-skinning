#include <glm/gtc/random.hpp>

#include "GPUParticleSystem.h"

const float TARGET_TIME = (float)(1.0 / 60.0);

GPUParticleSystem::GPUParticleSystem(sParticle * particleData, size_t particleCount, Shader &shader) :
m_shader(shader)
{
	assert(particleData);

	m_time = 0;
	m_va[0] = m_va[1] = 0;
	m_vb[0] = m_vb[1] = 0;
	m_target = 0;

	glGenVertexArrays(2, m_va);
	glGenBuffers(2, m_vb);

	int positionAttr 	= m_shader.getAttribLocation("in_position");
	int oldPositionAttr = m_shader.getAttribLocation("in_oldPosition");
	int massAttr 		= m_shader.getAttribLocation("in_mass_k_d");

	const char* pOffset = 0;

	glBindVertexArray(m_va[0]);
		glBindBuffer(GL_ARRAY_BUFFER, m_vb[0]);

		glEnableVertexAttribArray(positionAttr);
		glEnableVertexAttribArray(oldPositionAttr);
		glEnableVertexAttribArray(massAttr);

		glVertexAttribPointer(positionAttr, 3, GL_FLOAT, GL_FALSE, sizeof(sParticle), pOffset);
		glVertexAttribPointer(oldPositionAttr, 3, GL_FLOAT, GL_FALSE, sizeof(sParticle), 12 + pOffset);
		glVertexAttribPointer(massAttr, 3, GL_FLOAT, GL_FALSE, sizeof(sParticle), 24 + pOffset);
	glBindVertexArray(0);

	glBindVertexArray(m_va[1]);
		glBindBuffer(GL_ARRAY_BUFFER, m_vb[1]);

		glEnableVertexAttribArray(positionAttr);
		glEnableVertexAttribArray(oldPositionAttr);
		glEnableVertexAttribArray(massAttr);

		glVertexAttribPointer(positionAttr, 3, GL_FLOAT, GL_FALSE, sizeof(sParticle), pOffset);
		glVertexAttribPointer(oldPositionAttr, 3, GL_FLOAT, GL_FALSE, sizeof(sParticle), 12 + pOffset);
		glVertexAttribPointer(massAttr, 3, GL_FLOAT, GL_FALSE, sizeof(sParticle), 24 + pOffset);
	glBindVertexArray(0);

	setData(particleData, particleCount);
}

GPUParticleSystem::~GPUParticleSystem()
{
	glDeleteBuffers(2, m_vb);
}

void GPUParticleSystem::setData(sParticle * particleData, size_t particleCount)
{
	m_particleCount = particleCount;

	glBindBuffer(GL_ARRAY_BUFFER, m_vb[0]);
	glBufferData(GL_ARRAY_BUFFER, m_particleCount * sizeof(sParticle), particleData, GL_STREAM_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_vb[1]);
	glBufferData(GL_ARRAY_BUFFER, m_particleCount * sizeof(sParticle), particleData, GL_STREAM_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GPUParticleSystem::update(float dt)
{
	m_time += dt;

	//if (m_time < TARGET_TIME)
	//	return;

	m_time = 0;

	m_shader.bind();

	glEnable(GL_RASTERIZER_DISCARD);

	glBindVertexArray(getSourceVA());

	int loc = m_shader.getUniformLocation("dt");
	glUniform1f(loc, dt);

	m_shader.validate();

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, getTargetVB());

	// Perform GPU simulation:
	glBeginTransformFeedback(GL_POINTS);
	glDrawArrays(GL_POINTS, 0, (GLsizei)m_particleCount);
	glEndTransformFeedback();

	glBindVertexArray(0);

	glDisable(GL_RASTERIZER_DISCARD);

	// Swap the A and B buffers for ping-ponging
	swapTarget();
}