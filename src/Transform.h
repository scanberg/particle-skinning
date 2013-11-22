#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform
{
public:
	Transform();
	Transform(const glm::mat4 &matrix);

	void setTranslation(const glm::vec3 &translation) { m_translation = translation; }
	void setOrientation(const glm::quat &orientation) { m_orientation = orientation; }
	void setScale(const glm::vec3 &scale) { m_scale = scale; }

	void setMat4(const glm::mat4 &matrix);

	const glm::vec3 & getTranslation() { return m_translation; }
	const glm::quat & getOrientation() { return m_orientation; }
	const glm::vec3 & getScale() { return m_scale; }

	glm::mat4 getMat4();
	glm::mat4 getInvMat4();

private:
	glm::vec3 m_translation;
	glm::quat m_orientation;
	glm::vec3 m_scale;
};