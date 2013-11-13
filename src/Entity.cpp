#include "Entity.h"
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/quaternion.hpp>

void Entity::setPosition(const glm::vec3 & position)
{
	m_transform.setTranslation(position);
}

void Entity::setOrientation(const glm::quat & orientation)
{
	m_transform.setOrientation(orientation);
}

void Entity::setScale(const glm::vec3 & scale)
{
	m_transform.setScale(scale);
}

void Entity::translate(const glm::vec3 & translation)
{
	glm::vec3 position = m_transform.getTranslation();
	m_transform.setTranslation(position + translation);
}

void Entity::move(const glm::vec3 & move)
{
	// Maybe R-1 should be used
	glm::mat3 R = glm::mat3_cast(m_transform.getOrientation());
	translate(R * move);
}

void Entity::rotate(const glm::quat & quaternion)
{
	glm::quat orientation = m_transform.getOrientation();
	m_transform.setOrientation(orientation * quaternion);
}

void Entity::rotate(const glm::vec3 & eulerAngles)
{
	glm::vec3 xyz(eulerAngles.y, eulerAngles.x, eulerAngles.z);
	glm::mat3 R = glm::orientate3(xyz);
	rotate(glm::quat_cast(R));
}

Transform Entity::getTransform()
{
	return m_transform;
}