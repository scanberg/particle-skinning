#include <glm/gtc/type_ptr.hpp>

#include "ParticleSkinnedModel.h"

ParticleSkinnedModel::ParticleSkinnedModel(Shader * particleShader, Body * body, Material ** mat, unsigned int materialCount) :
Model(body, mat, materialCount)
{
	assert(particleShader);
	assert(body);

    size_t		particleCount = body->getVertexCount();
    sParticle * particleData = new sParticle[particleCount];

    for(size_t i=0; i<particleCount; ++i)
    {
        const Body::sVertex v = body->getVertexData()[i];
        sParticle& p = particleData[i];
		p.oldPosition = p.position = v.position;
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

void ParticleSkinnedModel::resetParticlePositions()
{
	const size_t max_bones = 128;
	glm::mat4 finalMat[max_bones];
	// 16K on the stack thanks to this, perhaps allocate in heap?
	// The idéa is to make sure it is coherent in memory.

	size_t boneCount = m_body->getBoneCount();
	assert(boneCount < max_bones);

	calculateFinalBoneMatrices(finalMat, boneCount);

    size_t		particleCount = m_body->getVertexCount();
    sParticle * particleData = new sParticle[particleCount];

    for(size_t i=0; i<particleCount; ++i)
    {
        const Body::sVertex v = m_body->getVertexData()[i];
		sParticle& p = particleData[i];

		glm::vec4 finalPos(0.0f);
		for(size_t u=0; u<4; ++u)
		{
			if(v.index[u] == -1)
				break;

			finalPos += v.weight[u] * finalMat[v.index[u]] * glm::vec4(v.position, 1);
		}

		//finalPos = getTransform().getMat4() * finalPos;
        
		p.oldPosition = p.position = glm::vec3(finalPos);
		p.mass = 0.1f;
    }

	m_ps->setData(particleData, particleCount);
	delete[] particleData;
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

	const int PS_SUB_UPDATE_STEPS = 2;
	const float TARGET_TIME = (float)(1.0 / (60.0 * PS_SUB_UPDATE_STEPS));
	for(int i=0; i<PS_SUB_UPDATE_STEPS; ++i)
		m_ps->update(TARGET_TIME);
}

void ParticleSkinnedModel::draw()
{
	const std::vector<Body::sPart>& partData = m_body->getParts();

    glBindVertexArray(m_ps->getTargetVA());
    //glBindVertexArray(m_body->getVertexArray());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_body->getIndexBuffer());

    for(size_t i=0; i<1; ++i)
    {
		GLuint 					offset 	= partData[i].offset * 3;
		GLsizei 				count 	= partData[i].count * 3;
		const unsigned int * 	pOffset = 0;

   		//glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, pOffset + offset);
	}

	glDrawElements(GL_TRIANGLES, 3 * m_body->getTriangleCount(), GL_UNSIGNED_INT, 0);
    //glPointSize(2);
    //glDrawArrays(GL_POINTS, 0, m_ps->getParticleCount());
    glBindVertexArray(0);
}