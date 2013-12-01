#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <assimp/scene.h>
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

	struct sVertex
	{
		sVertex() : index(glm::ivec4(-1)) {};
		glm::vec3 	position;
		glm::vec3 	normal;
		glm::vec2 	texCoord;
		glm::ivec4 	index;
		glm::vec4 	weight;
	};

	struct sTriangle
	{
		sTriangle() : index() {}
		unsigned int index[3];
	};

	struct sPart
	{
		sPart() : offset(0), count(0) {};
		sPart(int o, int c) : offset(o), count(c) {}
		unsigned int offset;
		unsigned int count;
	};

	Body(const char * meshfile);
	~Body();

	sVertex *		getVertexData() { return &m_vertexData[0]; }
	size_t 			getVertexCount() { return m_vertexData.size(); }

	sTriangle * 	getTriangleData() { return &m_triangleData[0]; }
	size_t			getTriangleCount() { return m_triangleData.size(); }

	size_t 			getPartCount() { return m_partData.size(); }
	size_t 			getBoneCount() { return m_boneData.size(); }
	size_t		 	getAnimationCount() { return m_animationData.size(); }

	sPart * 		getPart(const StringHash & sh);
	sPart *			getPart(size_t index);
	int				getPartIndex(const StringHash & sh);

	Transform * 	getBone(const StringHash & sh);
	Transform *		getBone(size_t index);
	int				getBoneIndex(const StringHash & sh);

	Animation * 	getAnimation(const StringHash & sh);
	Animation *		getAnimation(size_t index);
	int				getAnimationIndex(const StringHash & sh);

	unsigned int 	getVertexArray() { return m_va; }
	unsigned int 	getVertexBuffer() { return m_vb; }
	unsigned int	getIndexBuffer() { return m_ib; }

	void 			addAnimation(const char * animationfile, const char * name);
	void			draw();
	void 			drawPart(unsigned int index);
	
private:
	void readBoneHierarchy(aiNode * node, int parent);
	void fillBuffers();

	// GL buffers
	unsigned int 	m_va;
	unsigned int 	m_vb;
	unsigned int 	m_ib;

	// Geometric data
	std::vector<sVertex> 	m_vertexData;
	std::vector<sTriangle>	m_triangleData;

	// Groups defining parts of the mesh
	std::vector<StringHash>	m_partSH;
	std::vector<sPart>		m_partData;

	// Bone transformations
	std::vector<StringHash>	m_boneSH;
	std::vector<Transform> 	m_boneData;
	std::vector<int>		m_boneParent;

	// Animation data
	std::vector<StringHash> m_animationSH;
	std::vector<Animation>	m_animationData;
};
