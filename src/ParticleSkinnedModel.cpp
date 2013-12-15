#include <glm/gtc/type_ptr.hpp>

#include "ParticleSkinnedModel.h"

ParticleSkinnedModel::ParticleSkinnedModel(Shader& pShader, Body * body, Material ** mat, unsigned int materialCount) :
Model(body, mat, materialCount)
{
	assert(body);

	const std::vector<Body::sTriangle> tris = m_body->getTriangleData();
	std::set<Body::sLine> uniqueConnections;

	// add unique line connections given by triangles
	for(size_t i=0; i<body->getTriangleCount(); ++i)
	{
		uniqueConnections.insert(Body::sLine(tris[i].index[0], tris[i].index[1]));
		uniqueConnections.insert(Body::sLine(tris[i].index[0], tris[i].index[2]));
		uniqueConnections.insert(Body::sLine(tris[i].index[1], tris[i].index[2]));
	}

	for(std::set<Body::sLine>::const_iterator it = uniqueConnections.begin(); it != uniqueConnections.end(); ++it)
		m_constraints.push_back(*it);

	printf("Unique connections: %zu \n", m_constraints.size());

    size_t particleCount = body->getVertexCount();
	m_particles.resize(particleCount);

    for(size_t i=0; i<particleCount; ++i)
    {
        const Body::sVertex v = body->getVertexData()[i];
        sParticle& p = m_particles[i];
		p.oldPosition = p.position = v.position;
		p.mass = 0.1f;
    }

	m_ps = new GPUParticleSystem(&m_particles[0], particleCount, pShader);
	assert(m_ps);

	m_particleBuffer.bind();
	glBufferData(GL_ARRAY_BUFFER, particleCount * sizeof(sParticle), &m_particles[0], GL_STREAM_DRAW);
	m_particleBuffer.unbind();

	int positionAttr	= pShader.getAttribLocation("in_vertexPosition");
	int weightAttr 		= pShader.getAttribLocation("in_vertexWeight");

	printf("positionAttr %i\n",	positionAttr);
	printf("weightAttr %i\n",	weightAttr);

	const char * pOffset = 0;

	// Append Vertex Attribute pointers to the particle systems VAOs
	glBindVertexArray(m_ps->getSourceVA());

		glBindBuffer(GL_ARRAY_BUFFER, body->getVertexBuffer());

		glEnableVertexAttribArray(positionAttr);
		glEnableVertexAttribArray(weightAttr);

		glVertexAttribPointer(positionAttr, 3, GL_FLOAT, GL_FALSE, 	sizeof(Body::sVertex), pOffset);
		glVertexAttribPointer(weightAttr,	4, Body::VertexWeight::getGLType(), GL_FALSE, 	sizeof(Body::sVertex), 32 + pOffset);

	glBindVertexArray(0);

	glBindVertexArray(m_ps->getTargetVA());

		glBindBuffer(GL_ARRAY_BUFFER, body->getVertexBuffer());

		glEnableVertexAttribArray(positionAttr);
		glEnableVertexAttribArray(weightAttr);

		glVertexAttribPointer(positionAttr, 3, GL_FLOAT, GL_FALSE, 	sizeof(Body::sVertex), pOffset);
		glVertexAttribPointer(weightAttr,	4, Body::VertexWeight::getGLType(), GL_FALSE, 	sizeof(Body::sVertex), 32 + pOffset);

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

	for(size_t i=0; i<m_particles.size(); ++i)
    {
        const Body::sVertex v = m_body->getVertexData()[i];
		sParticle& p = m_particles[i];

		glm::vec4 finalPos(0.0f);
		for(size_t u=0; u<MAX_WEIGHTS; ++u)
		{
			if(v.weight[u].unused())
				break;

			int index =		v.weight[u].getIndex();
			float weight =	v.weight[u].getWeight();
			finalPos += weight * finalMat[index] * glm::vec4(v.position, 1);
		}
        
		p.oldPosition = p.position = glm::vec3(finalPos);
		p.mass = 0.1f;
    }

	m_ps->setData(&m_particles[0], m_particles.size());
}

void ParticleSkinnedModel::update(float dt)
{
	// Send update call to superclass
	Model::update(dt);

	const int PS_SUB_UPDATE_STEPS = 1;

	const float TARGET_TIME = (float)(1.0 / (60.0 * PS_SUB_UPDATE_STEPS));

	dt = TARGET_TIME;

	if(m_simulateOnGPU)
	{
		int loc;

		Shader& shader = m_ps->getShader();
		shader.bind();

		loc = shader.getUniformLocation("modelMatrix");
		glUniformMatrix4fv(loc, 1, false, glm::value_ptr(getTransform().getMat4()));

		loc = shader.getUniformLocation("invModelMatrix");
		glUniformMatrix4fv(loc, 1, false, glm::value_ptr(getTransform().getInvMat4()));

		loc = shader.getUniformLocation("boneMatrix");
		calculateAndSetBoneMatrices(loc);

		
		for(int i=0; i<PS_SUB_UPDATE_STEPS; ++i)
		{
			m_ps->update(dt);
		}
	}
	else
	{
		const size_t max_bones = 128;
		glm::mat4 finalMat[max_bones];
		// 16K on the stack thanks to this, perhaps allocate in heap?
		// The idéa is to make sure it is coherent in memory.

		std::vector<glm::vec3> targetPos(m_particles.size());

		size_t boneCount = m_body->getBoneCount();
		assert(boneCount < max_bones);

		calculateFinalBoneMatrices(finalMat, boneCount);

		for(size_t i=0; i<targetPos.size(); ++i)
		{
			const Body::sVertex& v = m_body->getVertexData()[i];

			glm::vec4 finalPos(0.0f);
			for(size_t u=0; u<MAX_WEIGHTS; ++u)
			{
				if(v.weight[u].unused())
					break;

				int index =		v.weight[u].getIndex();
				float weight =	v.weight[u].getWeight();
				finalPos += weight * finalMat[index] * glm::vec4(v.position, 1);
			}
			targetPos[i] = glm::vec3(finalPos);
		}

		const glm::vec3 externalForce(0);
		const glm::vec3 externalAcc(0,0,0);
		const float damping = 0.05;

		// Simulate timesteps
		for(size_t i=0; i<m_particles.size(); ++i)
		{
			sParticle& p = m_particles[i]; 
			glm::vec3& target = targetPos[i];

			glm::vec3 attrForce = (target - p.position) * 60.0f;

			glm::vec3 force = externalForce + attrForce;
			glm::vec3 acc = externalAcc + force / p.mass;

			glm::vec3 vel = (1.0f - damping) * (p.position - p.oldPosition) + acc * dt * dt;
			p.oldPosition = p.position;
			p.position += vel;

			//p.position = target;
		}

		//const std::vector<Body::sVertex>& vertexData = m_body->getVertexData();
		//const float stiffness = 0.01f;

		/*
		// Update constraints
		for(size_t i=0; i<m_constraints.size(); ++i)
		{
			size_t i0 = m_constraints[i].index[0];
			size_t i1 = m_constraints[i].index[1];

			sParticle& p0 = m_particles[i0];
			sParticle& p1 = m_particles[i1];

			glm::vec3 delta = p1.position - p0.position;

			glm::vec3 restDelta = vertexData[i1].position - vertexData[i0].position;
			float restLength2 = glm::dot(restDelta, restDelta);
            
			delta *= (1.0f - 2.0f * restLength2 / (restLength2 + glm::dot(delta,delta)) ) * (p0.mass + p1.mass);
            
			glm::vec3 val = stiffness * delta;
            
			p0.position += val / p0.mass;
			p1.position -= val / p1.mass;
		}
		*/

		// Update bufferdata!
		m_particleBuffer.bind();

		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(sParticle) * m_particles.size(), &m_particles[0]);

		m_particleBuffer.unbind();
	}
}

void ParticleSkinnedModel::draw()
{
	const std::vector<Body::sPart>& partData = m_body->getParts();

	for(size_t i=0; i<partData.size(); ++i)
		drawPart(i);
}

void ParticleSkinnedModel::drawPart(size_t index)
{
	const std::vector<Body::sPart>& partData = m_body->getParts();

	if(index >= partData.size())
		return;

	Shader& shad = *Shader::getBoundShader();

	int positionAttr 	= shad.getAttribLocation("in_vertexPosition");
	int normalAttr 		= shad.getAttribLocation("in_vertexNormal");
	int texCoordAttr 	= shad.getAttribLocation("in_vertexTexCoord");

	const char*		pOffset = 0;
	const GLsizei	offset = partData[index].offset * 3 * sizeof(unsigned int);
	const GLsizei	count = partData[index].count * 3;

	m_vertexArray.bind();
			
		if(m_simulateOnGPU)
			glBindBuffer(GL_ARRAY_BUFFER, m_ps->getTargetVB());
		else
			m_particleBuffer.bind();

		glEnableVertexAttribArray(positionAttr);
		glVertexAttribPointer(positionAttr, 3, GL_FLOAT, GL_FALSE, 	sizeof(sParticle), pOffset + 0);

		glBindBuffer(GL_ARRAY_BUFFER, m_body->getVertexBuffer());

		glEnableVertexAttribArray(normalAttr);
		glEnableVertexAttribArray(texCoordAttr);
			
		glVertexAttribPointer(normalAttr, 	3, GL_FLOAT, GL_FALSE, 	sizeof(Body::sVertex), pOffset + 12);
		glVertexAttribPointer(texCoordAttr, 2, GL_FLOAT, GL_FALSE, 	sizeof(Body::sVertex), pOffset + 24);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_body->getIndexBuffer());

   		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, pOffset + offset);

	m_vertexArray.unbind();
}