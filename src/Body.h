#pragma once

#include <glm/glm.hpp>
#include "Animation.h"
#include "StringHash.h"

/*	Mesh class
 *	Should be considered as a resource
 *
 *
 */
class Body
{
public:

	typedef struct
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoord;
	}sVertex;

	typedef struct
	{
		unsigned int index[3];
	}sTriangle;

	typedef struct
	{
		unsigned int offset;
		unsigned int count;
	}sPart;

	Body(const char * filename);
	~Body();

	sVertex *		getVertexData();
	unsigned int 	getVertexCount();

	sTriangle * 	getTriangleData();
	unsigned int	getTriangleCount();

	unsigned int 	getMeshCount();
	unsigned int 	getBoneCount();
	unsigned int 	getAnimationCount();

	sPart * 		getPart(const StringHash & sh);
	Trasform * 		getBone(const StringHash & sh);
	Animation * 	getAnimation(const StringHash & sh);
	
private:
	// GL buffers
	unsigned int 	m_vb;
	unsigned int 	m_ib;

	sVertex * 		m_vertexData;
	unsigned int 	m_vertexCount;

	sTriangle * 	m_triangleData;
	unsigned int 	m_triangleCount;

	unsigned int * 	m_partID;
	sPart *			m_partData;
	unsigned int 	m_partCount;

	unsigned int *	m_boneID;
	Transform * 	m_boneData;
	unsigned int 	m_boneCount;

	unsigned int * 	m_animationID;
	Animation *		m_animationData;
	unsigned int	m_animationCount;
};