#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(float fov, float near, float far, float aspectRatio)
{
	m_fov 			= fov;
	m_near 			= near;
	m_far 			= far;
	m_aspectRatio 	= aspectRatio;
}

glm::mat4 Camera::getViewMatrix()
{
	return m_transform.getInvMat4();
}

glm::mat4 Camera::getInvViewMatrix()
{
	return m_transform.getMat4();
}

glm::mat4 Camera::getProjMatrix()
{
	return glm::perspective(m_fov, m_aspectRatio, m_near, m_far);
}

glm::mat4 Camera::getInvProjMatrix()
{
	return glm::inverse(getProjMatrix());
}