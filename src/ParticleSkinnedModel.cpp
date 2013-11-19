#include <glm/gtc/type_ptr.hpp>

#include "ParticleSkinnedModel.h"

ParticleSkinnedModel::ParticleSkinnedModel(Shader * particleShader, Body * body, Material ** mat, unsigned int materialCount)
:
Model(body, mat, materialCount),
m_ps(body, particleShader)
{
	assert(particleShader);
	assert(body);

	int positionAttr 	= particleShader->getAttribLocation("in_vertexPosition");
	int normalAttr 		= particleShader->getAttribLocation("in_vertexNormal");
	int texCoordAttr 	= particleShader->getAttribLocation("in_vertexTexCoord");
	int indexAttr 		= particleShader->getAttribLocation("in_vertexIndex");
	int weightAttr 		= particleShader->getAttribLocation("in_vertexWeight");

	const char * pOffset = 0;

	glBindVertexArray(m_ps.getSourceVA());

		glBindBuffer(GL_ARRAY_BUFFER, body->getVertexBuffer());

		glEnableVertexAttribArray(positionAttr);
		glEnableVertexAttribArray(normalAttr);
		glEnableVertexAttribArray(texCoordAttr);
		glEnableVertexAttribArray(indexAttr);
		glEnableVertexAttribArray(weightAttr);

		glVertexAttribPointer(positionAttr, 3, GL_FLOAT, GL_FALSE, 	sizeof(Body::sVertex), pOffset);
		glVertexAttribPointer(normalAttr, 	3, GL_FLOAT, GL_FALSE, 	sizeof(Body::sVertex), 12 + pOffset);
		glVertexAttribPointer(texCoordAttr, 2, GL_FLOAT, GL_FALSE, 	sizeof(Body::sVertex), 24 + pOffset);
		glVertexAttribIPointer(indexAttr, 	4, GL_INT, 	 			sizeof(Body::sVertex), 32 + pOffset);
		glVertexAttribPointer(weightAttr, 	4, GL_FLOAT, GL_FALSE, 	sizeof(Body::sVertex), 48 + pOffset);

	glBindVertexArray(0);

	glBindVertexArray(m_ps.getTargetVA());

		glBindBuffer(GL_ARRAY_BUFFER, body->getVertexBuffer());

		glEnableVertexAttribArray(positionAttr);
		glEnableVertexAttribArray(normalAttr);
		glEnableVertexAttribArray(texCoordAttr);
		glEnableVertexAttribArray(indexAttr);
		glEnableVertexAttribArray(weightAttr);

		glVertexAttribPointer(positionAttr, 3, GL_FLOAT, GL_FALSE, 	sizeof(Body::sVertex), pOffset);
		glVertexAttribPointer(normalAttr, 	3, GL_FLOAT, GL_FALSE, 	sizeof(Body::sVertex), 12 + pOffset);
		glVertexAttribPointer(texCoordAttr, 2, GL_FLOAT, GL_FALSE, 	sizeof(Body::sVertex), 24 + pOffset);
		glVertexAttribIPointer(indexAttr, 	4, GL_INT, 	 			sizeof(Body::sVertex), 32 + pOffset);
		glVertexAttribPointer(weightAttr, 	4, GL_FLOAT, GL_FALSE, 	sizeof(Body::sVertex), 48 + pOffset);

	glBindVertexArray(0);
}

ParticleSkinnedModel::~ParticleSkinnedModel()
{
	
}

void ParticleSkinnedModel::update(float dt)
{
	int loc;

	Shader * shader = m_ps.getShader();

	m_ps.getShader()->bind();

	loc = shader->getUniformLocation("modelMatrix");
	glUniformMatrix4fv(loc, 1, false, glm::value_ptr(getTransform().getMat4()));

	loc = shader->getUniformLocation("invModelMatrix");
	glUniformMatrix4fv(loc, 1, false, glm::value_ptr(getTransform().getInvMat4()));

	m_ps.update(dt);
}

void ParticleSkinnedModel::draw()
{
    glBindVertexArray(m_ps.getTargetVA());
    //glBindVertexArray(m_body->getVertexArray());
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, b->getIndexBuffer());
    //glDrawElements(GL_TRIANGLES, 3 * b->getTriangleCount(), GL_UNSIGNED_INT, 0);

    glDrawArrays(GL_POINTS, 0, m_ps.getParticleCount());
    glBindVertexArray(0);
}