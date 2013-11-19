#include "GPUParticleSystem.h"

GPUParticleSystem::GPUParticleSystem(sParticle * particleData, unsigned int particleCount, Shader& shader)
{
	m_va[0] = m_va[1] = 0;
	m_vb[0] = m_vb[1] = 0;
	m_particleCount = particleCount;
	m_target = 0;
	m_shader = &shader;

	char* pOffset = 0;

	glGenVertexArrays(2, m_va);
	glGenBuffers(2, m_vb);

	int positionAttr 	= m_shader->getAttribLocation("in_position");
	int oldPositionAttr = m_shader->getAttribLocation("in_oldPosition");
	int massAttr 		= m_shader->getAttribLocation("in_mass");

	glBindVertexArray(m_va[0]);
		glBindBuffer(GL_ARRAY_BUFFER, m_vb[0]);
		glBufferData(GL_ARRAY_BUFFER, m_particleCount * sizeof(sParticle), particleData, GL_STREAM_DRAW);

		glEnableVertexAttribArray(positionAttr);
		glEnableVertexAttribArray(oldPositionAttr);
		glEnableVertexAttribArray(massAttr);

		glVertexAttribPointer(positionAttr, 3, GL_FLOAT, GL_FALSE, sizeof(sParticle), pOffset);
		glVertexAttribPointer(oldPositionAttr, 3, GL_FLOAT, GL_FALSE, sizeof(sParticle), 12 + pOffset);
		glVertexAttribPointer(massAttr, 1, GL_FLOAT, GL_FALSE, sizeof(sParticle), 24 + pOffset);

		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_vb[1]);
	glBindVertexArray(0);
	
	glBindVertexArray(m_va[1]);
		glBindBuffer(GL_ARRAY_BUFFER, m_vb[1]);
		glBufferData(GL_ARRAY_BUFFER, m_particleCount * sizeof(sParticle), 0, GL_STREAM_DRAW);

		glEnableVertexAttribArray(positionAttr);
		glEnableVertexAttribArray(oldPositionAttr);
		glEnableVertexAttribArray(massAttr);

		glVertexAttribPointer(positionAttr, 3, GL_FLOAT, GL_FALSE, sizeof(sParticle), pOffset);
		glVertexAttribPointer(oldPositionAttr, 3, GL_FLOAT, GL_FALSE, sizeof(sParticle), 12 + pOffset);
		glVertexAttribPointer(massAttr, 1, GL_FLOAT, GL_FALSE, sizeof(sParticle), 24 + pOffset);

		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_vb[0]);
	glBindVertexArray(0);
}

GPUParticleSystem::~GPUParticleSystem()
{
	glDeleteBuffers(2, m_vb);
}

void GPUParticleSystem::update(float dt)
{
	m_shader->bind();

	glEnable(GL_RASTERIZER_DISCARD);

	glBindVertexArray(m_va[m_target]);

	m_shader->validate();

	// Perform GPU advection:
	glBeginTransformFeedback(GL_POINTS);
	glDrawArrays(GL_POINTS, 0, m_particleCount);
	glEndTransformFeedback();

	glBindVertexArray(0);

	glDisable(GL_RASTERIZER_DISCARD);

	// Swap the A and B buffers for ping-ponging
	//swapTarget();
}