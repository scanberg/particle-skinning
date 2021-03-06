#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>

Transform::Transform() :
m_scale(glm::vec3(1))
{

}

Transform::Transform(const glm::vec3 &t, const glm::quat &o, const glm::vec3 &s) :
m_translation(t),
m_orientation(o),
m_scale(s)
{

}

Transform::Transform(const glm::mat4 &matrix) :
m_scale(glm::vec3(1))
{
	setMat4(matrix);
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

	/*printf("Result: S: %.2f %.2f %.2f, R: %.2f %.2f %.2f %.2f, T: %.2f %.2f %.2f\n",
		scale.x, scale.y, scale.z,
		orientation.w, orientation.x, orientation.y, orientation.z,
		translation.x, translation.y, translation.z);*/

	setScale(scale);
	setOrientation(orientation);
	setTranslation(translation);
}

glm::mat4 Transform::getMat4() const
{
	glm::mat4 T = glm::translate(glm::mat4(), m_translation);
	glm::mat4 R = glm::mat4_cast(m_orientation);
	glm::mat4 S = glm::scale(glm::mat4(), m_scale);

	return T * R * S;
}

glm::mat4 Transform::getInvMat4() const
{
	glm::mat4 T = glm::translate(glm::mat4(), -m_translation);
	glm::mat4 R = glm::mat4_cast(glm::conjugate(m_orientation));
	glm::mat4 S = glm::scale(glm::mat4(), 1.0f/m_scale);

	return S * R * T;
}

Transform Transform::interpolate(const Transform & t0, const Transform & t1, float k)
{
	Transform T;
	k = glm::clamp(k, 0.0f, 1.0f);

	T.setTranslation(glm::mix(t0.getTranslation(), t1.getTranslation(), k));
	T.setOrientation(glm::slerp(t0.getOrientation(), t1.getOrientation(), k));
	T.setScale(glm::mix(t0.getScale(), t1.getScale(), k));

	return T;
}