#pragma once

#include <glm/glm.hpp>

typedef struct
{
	glm::vec3 position;
	glm::vec3 oldPosition;
	glm::vec3 mass_k_d;
}sParticle;

class ParticleSystem
{
public:
	virtual ~ParticleSystem() {}
	virtual void update(float dt) = 0;
};