#include "Model.h"

Model::Model(Body * body, Material ** material, unsigned int materialCount)
{
	m_body = body;
}

Model::~Model()
{

}

// Animation functionality
void Model::setAnimation(const char * animation)
{

}

void Model::setAnimation(unsigned int index)
{

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
	glBindBuffer(GL_ARRAY_BUFFER, m_body.getVertexBuffer());
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(sVertex), (char*)NULL);
    glEnableVertexAttribArray(0);
}