#include <glm/gtc/random.hpp>

#include "GPUParticleSystem.h"

const float TARGET_TIME = (float)(1.0 / 60.0);

GPUParticleSystem::GPUParticleSystem(sParticle * particleData, unsigned int particleCount, Shader * shader)
{
	assert(shader);
	assert(particleData);

	m_time = 0;
	m_va[0] = m_va[1] = 0;
	m_vb[0] = m_vb[1] = 0;
	m_particleCount = particleCount;
	m_target = 0;
	m_shader = shader;	

	glGenVertexArrays(2, m_va);
	glGenBuffers(2, m_vb);

	initData(particleData, particleCount);
}

GPUParticleSystem::GPUParticleSystem(Body * body, Shader * shader)
{
	assert(body);
	assert(shader);

    unsigned int    particleCount = body->getVertexCount();
    sParticle *     particleData = new sParticle[particleCount];

    for(unsigned int i=0; i<particleCount; ++i)
    {
        const Body::sVertex v = body->getVertexData()[i];
        sParticle& p = particleData[i];
		p.oldPosition = p.position = v.position;
		p.mass = 0.1f;
    }

	m_time = 0;
	m_va[0] = m_va[1] = 0;
	m_vb[0] = m_vb[1] = 0;
	m_particleCount = particleCount;
	m_target = 0;
	m_shader = shader;

	glGenVertexArrays(2, m_va);
	glGenBuffers(2, m_vb);

	initData(particleData, particleCount);

	delete[] particleData;
}

GPUParticleSystem::~GPUParticleSystem()
{
	glDeleteBuffers(2, m_vb);
}

void GPUParticleSystem::addRandomImpulse(float impulse)
{
	m_randomForceAccumulator += impulse / TARGET_TIME;
}

void GPUParticleSystem::update(float dt)
{
	m_time += dt;

	//if (m_time < TARGET_TIME)
	//	return;

	m_time = 0;

	m_shader->bind();

	glEnable(GL_RASTERIZER_DISCARD);

	glBindVertexArray(m_va[m_target]);

	int loc = m_shader->getUniformLocation("dt");
	glUniform1f(loc, TARGET_TIME);

	loc = m_shader->getUniformLocation("randomForce");
	glUniform1f(loc, m_randomForceAccumulator);

	m_randomForceAccumulator = 0.0f;

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

void GPUParticleSystem::initData(sParticle * particleData, unsigned int particleCount)
{
	char* pOffset = 0;

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