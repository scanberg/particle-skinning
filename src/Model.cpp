#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include "Model.h"


Model::Model(Body * body, Material ** material, unsigned int materialCount) :
m_body(body),
m_animTime(0.0f),
m_currentAnim(NULL)
{
	assert(body);
	m_partMaterial.resize(m_body->getPartCount(), -1);
}

Model::~Model()
{

}

// Animation functionality
void Model::setAnimation(const char * animation)
{
	StringHash sh(animation);
	Animation * anim = m_body->getAnimation(sh);

	if (anim)
		m_currentAnim = anim;
}

void Model::setAnimation(size_t index)
{
	Animation * anim = m_body->getAnimation(index);

	if (anim)
		m_currentAnim = anim;
}

void Model::play()
{
	m_animPlaying = true;
}

void Model::play(float speed, int iterations)
{
	m_animSpeed = speed;
	m_animLoopCount = iterations;
	m_animPlaying = true;
}

void Model::pause()
{
	m_animPlaying = false;
}

void Model::stop()
{
	m_animTime = 0.0f;
}

void Model::draw()
{

}

void Model::drawPart(size_t index)
{

}

void Model::calculateAndSetBoneMatrices(int uniformLocation)
{
	if (uniformLocation < 0)
		return;

	const size_t max_bones = 128;
	glm::mat4 finalMat[max_bones];
	// 16K on the stack thanks to this, perhaps allocate in heap?
	// The id�a is to make sure it is coherent in memory.

	size_t boneCount = m_body->getBoneCount();
	assert(boneCount < max_bones);

	calculateFinalBoneMatrices(finalMat, boneCount);

	glUniformMatrix4fv(uniformLocation, (GLsizei)boneCount, GL_FALSE, glm::value_ptr(finalMat[0]));
}

// Assumes that the boneMatrices have atleast m_body->getBoneCount() matrices!
void Model::calculateFinalBoneMatrices(glm::mat4* boneMatrices, size_t boneCount)
{
	assert(boneMatrices);

	const size_t max_bones = 128;
	glm::mat4 localMat[max_bones];

	const std::vector<int>& boneParent = m_body->getBoneParents();
	const std::vector<Transform>& boneOffset = m_body->getBoneOffsets();

	// Update Local poses
	for (size_t i = 0; i < boneCount; ++i)
		localMat[i] = m_currentAnim->getPoseAtTime(i, m_animTime).getMat4();

	// Calculate Final poses
	for (size_t i = 0; i < boneCount; ++i)
	{
		boneMatrices[i] = localMat[i];
		int parent = boneParent[i];

		// Append parents matrix until the bone has no parent (-1)
		while (parent != -1)
		{
			boneMatrices[i] = localMat[parent] * boneMatrices[i];
			parent = boneParent[parent];
		}

		boneMatrices[i] = boneMatrices[i] * boneOffset[i].getMat4();
	}
}

void Model::update(float dt)
{
	if (m_currentAnim)
	{
		// advance animation;
		if (m_animPlaying)
			m_animTime += m_animSpeed * dt;

		// loop
		if (m_animTime > m_currentAnim->getDuration())
			m_animTime -= m_currentAnim->getDuration();
	}
}