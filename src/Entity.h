#pragma once

#include "Transform.h"

class Entity
{
public:
	// Spatial functionality
	void setPosition(const glm::vec3 & position);
	void setOrientation(const glm::quat & orientation);
	void setScale(const glm::vec3 & scale);

	void translate(const glm::vec3 & translation);
	void move(const glm::vec3 & move);

	void rotate(const glm::quat & quaternion);
	void rotate(const glm::vec3 & eulerAngles);

	Transform getTransform() { return m_transform; }
protected:
	Transform m_transform;
};