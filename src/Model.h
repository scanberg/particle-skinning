#pragma once

#include "Body.h"
#include "Material.h"
#include "Entity.h"

/*
 *	Model
 *	
 *	Creates an instance of an entity that uses a mesh and material
 *	to form a renderable model to use in the scene.
 */
class Model : public Entity
{
public:
	Model(Body * body, Material ** material = NULL, unsigned int materialCount = 0);
	virtual ~Model();

	// Animation functionality
	void setAnimation(const char * animationName);
	void setAnimation(size_t index);

	void play(float speed = 1.0f, int iterations = 0);

	void pause();
	void stop();

	void assignMaterialToPart(Material * mat, StringHash part);
	void assignMaterialToAll(Material * mat);
	void assignMaterial(const char * filename);

	void draw();

	virtual void update(float dt);

	Body * 							getBody() { return m_body; }
	const std::vector<Material*>&	getMaterial() { return m_material; }
	size_t 							getMaterialCount() { return m_material.size(); }

protected:
	void calculateAndSetBoneMatrices(int uniformLocation);

	// Visuals
	Body *			m_body;
	std::vector<Material*>	m_material;

	// Animation
	bool			m_animPlaying;
	float			m_animTime;
	float			m_animSpeed;
	int				m_animLoopCount;
	Animation*		m_currentAnim;
};