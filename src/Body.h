#pragma once

#include <glm/glm.hpp>
#include "Animation.h"
#include "StringHash.h"

#include <assimp/cimport.h>        // Plain-C interface
#include <assimp/scene.h>          // Output data structure
#include <assimp/postprocess.h>    // Post processing flags


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
		glm::vec3 	position;
		glm::vec3 	normal;
		glm::vec2 	texCoord;
		glm::ivec4 	index;
		glm::vec4 	weight;
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

	Body(const char * meshfile, const char * animfile = NULL);
	~Body();

	bool ImportScene( const aiScene* scene);

	sVertex *		getVertexData();
	unsigned int 	getVertexCount();

	sTriangle * 	getTriangleData();
	unsigned int	getTriangleCount();

	unsigned int 	getMeshCount();
	unsigned int 	getBoneCount();
	unsigned int 	getAnimationCount();

	sPart * 		getPart(const StringHash & sh);
	Transform * 	getBone(const StringHash & sh);
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
