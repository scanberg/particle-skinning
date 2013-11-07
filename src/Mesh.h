#pragma once

#include <glm/glm.hpp>
#include "Transform.h"

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
	Transform 		pose;
	unsigned int 	frameNumber;
}sAnimationKeyFrame;

typedef struct
{
	sAnimationKeyFrame * 	keyFrame;
	unsigned int 			keyFrameCount;
}sAnimationChannel;

typedef struct
{
	sAnimationChannel * 	channel;
	unsigned int			channelCount;

	unsigned int 			framesPerSecond;
}sAnimation;

class Mesh
{
public:
	Mesh(const char * filename);
	~Mesh();

	sVertex *		getVertexData();
	unsigned int 	getVertexCount();

	sTriangle * 	getTriangleData();
	unsigned int	getTriangleCount();
private:
	sVertex * 		m_vertexData;
	unsigned int 	m_vertexCount;

	sTriangle * 	m_triangleData;
	unsigned int 	m_triangleCount;

	unsigned int	m_boneID;
	Transform * 	m_boneData;
	unsigned int 	m_boneCount;
};