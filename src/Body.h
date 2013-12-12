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
		sVertex() : index(glm::ivec4(-1)), weight(glm::vec4(0.0f)) {}
		glm::vec3 	position;
		glm::vec3 	normal;
		glm::vec2 	texCoord;
		glm::uvec4 	index;
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

	struct sMaterial
	{
		sMaterial(const char * d, const char * n, const char * h, const char * s) :
		diffuse(d), normal(n), height(h), specular(s) {}
		StringHash diffuse;
		StringHash normal;
		StringHash height;
		StringHash specular;
	};

	Body(const char * meshfile);
	~Body();

	const std::vector<sPart>& getParts() { return m_partData; }

	size_t 			getPartCount() { return m_partData.size(); }
	sPart * 		getPart(const StringHash & sh);
	sPart *			getPart(size_t index);
	int				getPartIndex(const StringHash & sh);

	const std::vector<sMaterial>& getMaterials() { return m_material; }

	/* Bone */
	const std::vector<Transform>&	getBoneOffsets() { return m_boneOffset; }
	const std::vector<int>&			getBoneParents() { return m_boneParent; }

	size_t 			getBoneCount() { return m_boneOffset.size(); }
	Transform * 	getBone(const StringHash & sh);
	Transform *		getBone(size_t index);
	int				getBoneIndex(const StringHash & sh);

	/* Animation */
	const std::vector<Animation>& getAnimations() { return m_animationData; }

	size_t		 	getAnimationCount() { return m_animationData.size(); }
	Animation * 	getAnimation(const StringHash & sh);
	Animation *		getAnimation(size_t index);
	int				getAnimationIndex(const StringHash & sh);

	/* Geometry */
	sVertex *		getVertexData() { return &m_vertexData[0]; }
	size_t 			getVertexCount() { return m_vertexData.size(); }

	sTriangle * 	getTriangleData() { return &m_triangleData[0]; }
	size_t			getTriangleCount() { return m_triangleData.size(); }

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
	std::vector<sMaterial> 	m_material;

	// Bone transformations
	std::vector<StringHash>	m_boneSH;
	std::vector<Transform> 	m_boneOffset;
	std::vector<int>		m_boneParent;

	// Animation data
	std::vector<StringHash> m_animationSH;
	std::vector<Animation>	m_animationData;
};
