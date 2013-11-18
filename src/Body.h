#pragma once

#include <glm/glm.hpp>
#include "Animation.h"
#include "StringHash.h"

/*	Body class
 *	Should be considered as a shared resource
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

	sVertex *		getVertexData() { return m_vertexData; }
	unsigned int 	getVertexCount() { return m_vertexCount; }

	sTriangle * 	getTriangleData() { return m_triangleData; }
	unsigned int	getTriangleCount() { return m_triangleCount; }

	unsigned int 	getPartCount() { return m_partCount; }
	unsigned int 	getBoneCount() { return m_boneCount; }
	unsigned int 	getAnimationCount() { return m_animationCount; }

	sPart * 		getPart(const StringHash & sh);
	Transform * 	getBone(const StringHash & sh);
	Animation * 	getAnimation(const StringHash & sh);

	unsigned int 	getVertexArray() { return m_va; }
	unsigned int 	getVertexBuffer() { return m_vb; }
	unsigned int	getIndexBuffer() { return m_ib; }

	void			draw();
	void 			drawPart(unsigned int index);
	
private:
	void fillBuffers();

	// GL buffers
	unsigned int 	m_va;
	unsigned int 	m_vb;
	unsigned int 	m_ib;

	// Geometric data
	sVertex * 		m_vertexData;
	unsigned int 	m_vertexCount;

	sTriangle * 	m_triangleData;
	unsigned int 	m_triangleCount;

	// Groups defining parts of the mesh
	StringHash * 	m_partSH;
	sPart *			m_partData;
	unsigned int 	m_partCount;

	// Bone transformations
	StringHash *	m_boneSH;
	Transform * 	m_boneData;
	unsigned int 	m_boneCount;

	// Animation data
	StringHash * 	m_animationSH;
	Animation *		m_animationData;
	unsigned int	m_animationCount;
};
