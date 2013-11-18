#include "GPUParticleSystem.h"

GPUParticleSystem::GPUParticleSystem(sParticle * particleData, unsigned int particleCount, Shader& shader)
{
	glGenBuffers(2, m_vb);
	m_particleCount = particleCount;
	m_target = 1;
	m_shader = &shader;

	glBindBuffer(GL_ARRAY_BUFFER, m_vb[0]);
	glBufferData(GL_ARRAY_BUFFER, m_particleCount * sizeof(sParticle), particleData, GL_STREAM_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_vb[1]);
	glBufferData(GL_ARRAY_BUFFER, m_particleCount * sizeof(sParticle), particleData, GL_STREAM_DRAW);
}

GPUParticleSystem::~GPUParticleSystem()
{
	glDeleteBuffers(2, m_vb);
}

void GPUParticleSystem::update(float dt)
{
	glEnable(GL_RASTERIZER_DISCARD);
	glBindBuffer(GL_ARRAY_BUFFER, getSourceVB());
	 
	// Specify the target buffer:
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, getTargetVB());

	int positionAttr 	= m_shader->getAttribLocation("in_position");
	int oldPositionAttr = m_shader->getAttribLocation("in_oldPosition");
	int massAttr 		= m_shader->getAttribLocation("in_mass");

	glEnableVertexAttribArray(positionAttr);
	glEnableVertexAttribArray(oldPositionAttr);
	glEnableVertexAttribArray(massAttr);

	char* pOffset = 0;
	glVertexAttribPointer(positionAttr, 3, GL_FLOAT, GL_FALSE, 16, pOffset);
	glVertexAttribPointer(oldPositionAttr, 3, GL_FLOAT, GL_FALSE, 16, 12 + pOffset);
	glVertexAttribPointer(massAttr, 1, GL_FLOAT, GL_FALSE, 16, 24 + pOffset);

	// Perform GPU advection:
	glBeginTransformFeedback(GL_POINTS);
	glDrawArrays(GL_POINTS, 0, m_particleCount);
	glEndTransformFeedback();

	// Swap the A and B buffers for ping-ponging, then turn the rasterizer back on:
	swapTarget();
	glDisable(GL_RASTERIZER_DISCARD);

}