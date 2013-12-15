#pragma once

#include "Body.h"
#include "Material.h"
#include "Entity.h"
#include "GL.h"

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

	void play();
	void play(float speed, int iterations = 0);

	void pause();
	void stop();

	bool isPlaying() { return m_animPlaying; }

	void assignMaterialToPart(Material * mat, StringHash part);
	void assignMaterialToPart(Material * mat, size_t index);
	void assignMaterialToAll(Material * mat);

	virtual void update(float dt);

	virtual void draw();
	virtual void drawPart(size_t index);

	Body* getBody() { return m_body; }

/*
	Material*						getMaterial(size_t i) { return material[i]; }
	const std::vector<Material*>&	getMaterials() { return m_partMaterial; }
	size_t 							getMaterialCount() { return m_partMaterial.size(); }
*/

protected:
	void calculateAndSetBoneMatrices(int uniformLocation);
	void calculateFinalBoneMatrices(glm::mat4* boneMatrices, size_t boneCount);

	// Body of the model
	Body *						m_body;

	// materials used, and a vertex array for that setup
	std::vector<Material*>		m_material;

	// Vertex array
	VertexArray					m_vertexArray;

	// part - material mapping
	std::vector<int>			m_partMaterial;

	// Animation
	bool			m_animPlaying;
	float			m_animTime;
	float			m_animSpeed;
	int				m_animLoopCount;
	Animation*		m_currentAnim;
};