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

	glm::vec3 boundingBoxMin(1.0e20f);
	glm::vec3 boundingBoxMax(-1.0e20f);
    for(size_t i=0; i<particleCount; ++i)
    {
        const Body::sVertex v = body->getVertexData()[i];
        sParticle& p 	= m_particles[i];
        p.position 		= v.position;
		p.oldPosition 	= p.position;
		p.mass_k_d 		= getParticleMass_K_D(i);

		boundingBoxMin = glm::min(boundingBoxMin, v.position);
		boundingBoxMax = glm::max(boundingBoxMax, v.position);
    }
	boundingBoxMin = boundingBoxMax - boundingBoxMin;
	printf("%f, %f, %f\n", boundingBoxMin.x, boundingBoxMin.y, boundingBoxMin.z);

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
		//p.mass = 0.1f;
    }

	m_ps->setData(&m_particles[0], m_particles.size());
}

void ParticleSkinnedModel::update(float dt)
{
	// Send update call to superclass
	Model::update(dt);

	const int PS_SUB_UPDATE_STEPS = 2;

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
		const glm::vec3 externalAcc(0,-9.82,0);
		const float maxDist = 0.1;

		for(int u=0; u<PS_SUB_UPDATE_STEPS; ++u)
		{
			// Simulate timesteps
			for(size_t i=0; i<m_particles.size(); ++i)
			{
				sParticle& p = m_particles[i]; 
				glm::vec3& target = targetPos[i];
				float mass 	= p.mass_k_d.x;
				float k 	= p.mass_k_d.y;
				float d 	= p.mass_k_d.z;

				float len 	= glm::length(target - p.position);

				glm::vec3 attrForce = (target - p.position) * k;

				glm::vec3 force = externalForce + attrForce;
				glm::vec3 acc = externalAcc + force / mass;

				glm::vec3 vel = (1.0f - d) * (p.position - p.oldPosition) + acc * dt * dt;
				p.oldPosition = p.position;
				p.position += vel;

				//p.position = target;
				//const Body::sVertex& v = m_body->getVertexData()[i];
				//p.position = v.position;
			}
		}

		/*
		const std::vector<Body::sVertex>& vertexData = m_body->getVertexData();
		const float stiffness = 0.01f;

		// Update constraints
		for(size_t i=0; i<m_constraints.size(); ++i)
		{
			size_t i0 = m_constraints[i].index[0];
			size_t i1 = m_constraints[i].index[1];

			sParticle& p0 = m_particles[i0];
			sParticle& p1 = m_particles[i1];

			float m0 = p0.mass_k_d.x;
			float m1 = p1.mass_k_d.x;

			glm::vec3 delta = p1.position - p0.position;

			glm::vec3 restDelta = vertexData[i1].position - vertexData[i0].position;
			float restLength2 = glm::dot(restDelta, restDelta);
            
			delta *= (1.0f - 2.0f * restLength2 / (restLength2 + glm::dot(delta,delta)) ) * (m0 + m1);
            
			glm::vec3 val = stiffness * delta;
            
			p0.position += val / m0;
			p1.position -= val / m1;
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
glm::vec3 ParticleSkinnedModel::getParticleMass_K_D(size_t index)
{
	float dist = distanceToBone(index);

	float mass = 0.1f;
	float k = glm::clamp(80.0f + 20.0f / dist, 80.0f, 150.0f);
	float d = glm::clamp(0.1f + 0.1f / glm::pow(dist, 0.3f), 0.1f, 0.4f);

	// Ballpark stats
	// mass = 0.1f;
	// k = 60.0f;
	// d = glm::clamp(0.1f / glm::pow(dist, 0.5f), 0.1f, 0.4f);

	return glm::vec3(mass, k, d);
}

float ParticleSkinnedModel::distanceToBone(size_t index)
{
	const std::vector<Body::sVertex>& vertexData = m_body->getVertexData();
	const std::vector<Transform>& boneData = m_body->getBoneOffsets();
	int weightCount = vertexData[index].getWeightCount();

	if(weightCount == 0){
		return 0.0f;
	} else if(weightCount == 1){
		glm::vec3 p(vertexData[index].position);
		int bindex = vertexData[index].weight[0].getIndex();
		//p = boneData[bindex].getMat4() * p;
		glm::vec3 b = glm::mat3_cast(boneData[bindex].getOrientation()) * boneData[bindex].getTranslation();
		//printf("b: %.2f,%.2f,%.2f;p: %.2f,%.2f,%.2f \n", b.x, b.y, b.z, p.x, p.y, p.z);
		return glm::length(b-p);
	} else {
		int bfi0 = -1, bfi1 = -1;
		float bff0 = 0.0, bff1 = 0.0;
		for(int i=0; i<4; ++i){
			if(bff0 < vertexData[index].weight[i].getWeight())
				bfi0 = i;
		}
		bff0 = vertexData[index].weight[bfi0].getWeight();
		for(int i=0; i<4; ++i){
			if(i != bfi0 && vertexData[index].weight[bfi1].getWeight() < vertexData[index].weight[i].getWeight())
				bfi1 = i;
		}
		bff1 = vertexData[index].weight[bfi1].getWeight();

		bfi0 = vertexData[index].weight[bfi0].getIndex();
		bfi1 = vertexData[index].weight[bfi1].getIndex();
		glm::vec3 b0 = glm::mat3_cast(boneData[bfi0].getOrientation()) * boneData[bfi0].getTranslation();
		glm::vec3 b1 = glm::mat3_cast(boneData[bfi1].getOrientation()) * boneData[bfi1].getTranslation();
		glm::vec3 p = vertexData[index].position;
		glm::vec3 v = b1 - b0;
		glm::vec3 u = p - b0;
		float dist = glm::length(u - glm::dot(u,v)/glm::length(v));
		//printf("%f\n", b0.y);
		//printf("%d: %f,%f,%f,%f \n", weightCount, vertexData[index].weight[0].getWeight() ,vertexData[index].weight[1].getWeight(), vertexData[index].weight[2].getWeight(),vertexData[index].weight[3].getWeight());

		return dist;
	}

}
