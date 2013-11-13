#pragma once

#include <glm/glm.hpp>
#include "Body.h"
#include "Material.h"
#include "Entity.h"

/*
 *	Model
 *	
 *	Creates an instance of an entity that uses a mesh and material
 *	to form a renderable model to use in the scene.
 */
class Model : public Entity
{
public:
	Model(Body * body, Material ** material = NULL, unsigned int materialCount = 0);
	~Model();

	// Animation functionality
	void setAnimation(const char * animation);
	void setAnimation(unsigned int index);

	void play(float speed = 1.0f, int iterations = 1);

	void pause();
	void stop();

	void assignMaterialToPart(Material * mat, StringHash part);
	void assignMaterialToAll(Material * mat);
	void assignMaterial(const char * filename);

	void draw();

private:
	Transform		m_transform;

	Body *			m_body;

	Material **		m_material;
	unsigned int 	m_materialCount;
};