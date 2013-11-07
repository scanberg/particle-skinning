#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>

Transform::Transform()
{
	m_scale = glm::vec3(1);
}

void Transform::setTranslation(const glm::vec3 &translation)
{
	m_translation = translation;
}

void Transform::setOrientation(const glm::quat &orientation)
{
	m_orientation = orientation;
}

void Transform::setScale(const glm::vec3 &scale)
{
	m_scale = scale;
}

void Transform::setMat4(const glm::mat4 &matrix)
{

}

const glm::vec3 & Transform::getTranslation()
{
	return m_translation;
}

const glm::quat & Transform::getOrientation()
{
	return m_orientation;
}

const glm::vec3 & Transform::getScale()
{
	return m_scale;
}

glm::mat4 Transform::getMat4()
{
	glm::mat4 T = glm::translate(glm::mat4(), m_translation);
	glm::mat4 R = glm::mat4_cast(m_orientation);
	glm::mat4 S = glm::scale(glm::mat4(), m_scale);

	return T * R * S;
}