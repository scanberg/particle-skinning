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
	glm::vec3 c[] = {
		glm::vec3(glm::column(matrix, 0)),
		glm::vec3(glm::column(matrix, 1)),
		glm::vec3(glm::column(matrix, 2)),
		glm::vec3(glm::column(matrix, 3)) };

	glm::vec3 scale = glm::vec3(glm::length(c[0]), glm::length(c[1]), glm::length(c[2]));
	glm::quat orientation = glm::quat_cast(matrix);
	glm::vec3 translation = c[3];

	setScale(scale);
	setOrientation(orientation);
	setTranslation(translation);
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