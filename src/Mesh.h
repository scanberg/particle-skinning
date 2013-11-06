#pragma once

#include <glm/glm.hpp>

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
};