#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform
{
public:
	Transform();

	void setTranslation(const glm::vec3 &translation);
	void setOrientation(const glm::quat &orientation);
	void setScale(const glm::vec3 &scale);

	void setMat4(const glm::mat4 &matrix);

	const glm::vec3 & getTranslation();
	const glm::quat & getOrientation();
	const glm::vec3 & getScale();

	glm::mat4 getMat4();
	glm::mat4 getInvMat4();

private:
	glm::vec3 m_translation;
	glm::quat m_orientation;
	glm::vec3 m_scale;
};