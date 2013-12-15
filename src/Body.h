#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <assimp/scene.h>
#include "Animation.h"
#include "StringHash.h"

#define MAX_WEIGHTS 4

/*	Body class
 *	Should be considered as a shared resource
 *
 *
 */
class Body
{
public:

	class VertexWeight
	{
	public:
		static GLenum	getGLType() { return GL_FLOAT; }

		VertexWeight() : data(0.0f) {}
		void	setIndex(int index) { data = index + glm::fract(data); }
		void	setWeight(float weight) { data = (int)data + weight * 0.99f; }
		void	set(int index, float weight) { data = index + weight * 0.99f; }

		float	getWeight() const { return glm::fract(data)/0.99f; }
		int		getIndex() const { return (int)data; }
		bool	unused() const { return data == 0.0f; }
	private:
		float data;
	};

	struct sVertex
	{
		glm::vec3 		position;
		glm::vec3 		normal;
		glm::vec2 		texCoord;
		VertexWeight	weight[MAX_WEIGHTS];
	};

	struct sLine
	{
		sLine();
		sLine(unsigned int indexA, unsigned indexB)
		{
			if(indexA < indexB)
			{
				index[0] = indexA;
				index[1] = indexB;
			}
			else
			{
				index[0] = indexB;
				index[1] = indexA;
			}
		}

		bool operator<(const sLine& rhs) const { return index[0] < rhs.index[0]; }

		unsigned int index[2];
	};

	struct sTriangle
	{
		sTriangle() { index[0] = index[1] = index[2] = 0; }
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
	const std::vector<sVertex>& 	getVertexData() { return m_vertexData; }
	size_t 							getVertexCount() { return m_vertexData.size(); }

	const std::vector<sTriangle>& 	getTriangleData() { return m_triangleData; }
	size_t							getTriangleCount() { return m_triangleData.size(); }

	unsigned int 	getVertexBuffer() { return m_vb; }
	unsigned int	getIndexBuffer() { return m_ib; }

	void 			addAnimation(const char * animationfile, const char * name);
	
private:
	void readBoneHierarchy(aiNode * node, int parent);
	void fillBuffers();

	// GL buffers
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
