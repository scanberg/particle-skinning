#pragma once

#include <glm/glm.hpp>
#include "Mesh.h"
#include "Material.h"

class Model
{
public:
	Model();
	~Model();

	void translate(const glm::vec3 & translation);
	void move(const glm::vec3 & move);

	void rotate(const glm::quat & quaternion);
	void rotate(const glm::vec3 & eulerAngles);

private:
	Transform	m_transform;

	Mesh *		m_mesh;
	Material *	m_material;
};