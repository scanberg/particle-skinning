#pragma once

#include <glm/glm.hpp>
#include "Body.h"
#include "Material.h"

/*
 *	Model
 *	
 *	Creates an instance of an entity that uses a mesh and material
 *	to form a renderable model to use in the scene.
 */
class Model
{
public:
	Model(Body * body, Material * material);
	~Model();

	// Spatial functionality
	void setPosition(const glm::vec3 & position);
	void setOrientation(const glm::quat & orientation);
	void setScale(const glm::vec3 & scale);

	void translate(const glm::vec3 & translation);
	void move(const glm::vec3 & move);

	void rotate(const glm::quat & quaternion);
	void rotate(const glm::vec3 & eulerAngles);

	// Animation functionality
	void setAnimation(const char * animation);
	void setAnimation(unsigned int index);

	void play(float speed = 1.0f, int iterations = 1);

	void pause();
	void stop();

	
private:
	Transform		m_transform;

	Body *			m_body;

	Material *		m_material;
	unsigned int 	m_materialCount;
};