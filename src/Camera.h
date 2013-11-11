#pragma once

#include "Entity.h"

class Camera : public Entity
{
public:
	Camera(float fov = 60.0f, float near = 0.1f, float far = 100.0f, float aspectRatio = 4.0/3.0);

	void lookAt(const glm::vec3 & position);

	glm::mat4 getViewMatrix();
	glm::mat4 getInvViewMatrix();

	glm::mat4 getProjMatrix();
	glm::mat4 getInvProjMatrix();

private:
	glm::mat4	m_projMatrix;

	float 		m_fov;
	float 		m_near;
	float 		m_far;
	float 		m_aspectRatio;
};