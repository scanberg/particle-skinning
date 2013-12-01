#include <GL/glew.h>

#include "Model.h"


Model::Model(Body * body, Material ** material, unsigned int materialCount) :
m_body(body),
m_animTime(0.0f),
m_currentAnim(NULL)
{
	assert(body);
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

void Model::play(float speed, int iterations)
{

}

void Model::pause()
{

}

void Model::stop()
{

}

void Model::assignMaterialToPart(Material * mat, StringHash part){}
void Model::assignMaterialToAll(Material * mat){}
void Model::assignMaterial(const char * filename)
{

}

void Model::draw()
{
	m_body->draw();
}