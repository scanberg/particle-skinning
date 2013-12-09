#include <glm/gtc/type_ptr.hpp>

#include "ParticleSkinnedModel.h"

ParticleSkinnedModel::ParticleSkinnedModel(Shader * particleShader, Body * body, Material ** mat, unsigned int materialCount) :
Model(body, mat, materialCount)
{
	assert(particleShader);
	assert(body);

    size_t		particleCount = body->getVertexCount();
    sParticle * particleData = new sParticle[particleCount];

	Transform T;
	T.setScale(glm::vec3(0.07));

    for(size_t i=0; i<particleCount; ++i)
    {
        const Body::sVertex v = body->getVertexData()[i];
        sParticle& p = particleData[i];
		p.oldPosition = p.position = glm::vec3(T.getMat4() * glm::vec4(v.position, 1));
		p.mass = 0.1f;
    }

	m_ps = new GPUParticleSystem(particleData, particleCount, particleShader);
	assert(m_ps);

	delete[] particleData;

	int positionAttr 	= particleShader->getAttribLocation("in_vertexPosition");
	int normalAttr 		= particleShader->getAttribLocation("in_vertexNormal");
	int texCoordAttr 	= particleShader->getAttribLocation("in_vertexTexCoord");
	int indexAttr 		= particleShader->getAttribLocation("in_vertexIndex");
	int weightAttr 		= particleShader->getAttribLocation("in_vertexWeight");

	const char * pOffset = 0;

	glBindVertexArray(m_ps->getSourceVA());

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

	glBindVertexArray(m_ps->getTargetVA());

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
	delete m_ps;
}

void ParticleSkinnedModel::update(float dt)
{
	// Send update call to superclass
	Model::update(dt);

	int loc;

	Shader * shader = m_ps->getShader();

	m_ps->getShader()->bind();

	loc = shader->getUniformLocation("modelMatrix");
	glUniformMatrix4fv(loc, 1, false, glm::value_ptr(getTransform().getMat4()));

	loc = shader->getUniformLocation("invModelMatrix");
	glUniformMatrix4fv(loc, 1, false, glm::value_ptr(getTransform().getInvMat4()));

	loc = shader->getUniformLocation("boneMatrix");
	calculateAndSetBoneMatrices(loc);

	//printf("animation time %.2f\n",m_animTime);

	m_ps->update(dt);
}

void ParticleSkinnedModel::draw()
{
    glBindVertexArray(m_ps->getTargetVA());
    //glBindVertexArray(m_body->getVertexArray());
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, b->getIndexBuffer());
    //glDrawElements(GL_TRIANGLES, 3 * b->getTriangleCount(), GL_UNSIGNED_INT, 0);

    glPointSize(2);
    glDrawArrays(GL_POINTS, 0, m_ps->getParticleCount());
    glBindVertexArray(0);
}