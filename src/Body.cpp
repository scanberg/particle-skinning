#include "Body.h"

#include <assimp/cimport.h>        // Plain-C interface
#include <assimp/scene.h>          // Output data structure
#include <assimp/postprocess.h>    // Post processing flags
#include <glm/gtc/type_ptr.hpp>    // aiMatrix4 -> glm:mat4
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>

inline void copyAiMatrixToGLM(const aiMatrix4x4 *from, glm::mat4 &to)
{
        to[0][0] = (GLfloat)from->a1; to[1][0] = (GLfloat)from->a2;
        to[2][0] = (GLfloat)from->a3; to[3][0] = (GLfloat)from->a4;
        to[0][1] = (GLfloat)from->b1; to[1][1] = (GLfloat)from->b2;
        to[2][1] = (GLfloat)from->b3; to[3][1] = (GLfloat)from->b4;
        to[0][2] = (GLfloat)from->c1; to[1][2] = (GLfloat)from->c2;
        to[2][2] = (GLfloat)from->c3; to[3][2] = (GLfloat)from->c4;
        to[0][3] = (GLfloat)from->d1; to[1][3] = (GLfloat)from->d2;
        to[2][3] = (GLfloat)from->d3; to[3][3] = (GLfloat)from->d4;
}

int findStringHashInVector(const StringHash & sh, const std::vector<StringHash> & v)
{
	for (int i = 0; i < (int)v.size(); ++i)
	{
		if (v[i] == sh)
			return i;
	}

	// not found
	return -1;
}

void printNode(aiNode* node, size_t level)
{
	if (!node)
		return;

	for (size_t i = 0; i < level; ++i)
		printf("  ");
	printf("%s\n", node->mName.C_Str());

	for (size_t i = 0; i < node->mNumChildren; ++i)
		printNode(node->mChildren[i], level+1);
}

void insertWeight(Body::sVertex & v, unsigned int index, float weight)
{
	for(int i=0; i<MAX_WEIGHTS; ++i)
	{
		if(v.weight[i].unused()) {
			v.weight[i].set(index, weight);
			return;
		}
	}

	// If we end up here, we're in trouble, no more slots for the weight
	// Strategy: kick out the least important one and renormalize weights
	int		leastI = -1;
	float	leastW = weight;

	for(int i=0; i<MAX_WEIGHTS; ++i)
	{
		float w = v.weight[i].getWeight();
		if(w < leastW)
		{
			leastI = i;
			leastW = w;
		}
	}

	// If an existing weight was the least
	if(leastI > -1)
	{
		// Swap with the new weight
		v.weight[leastI].set(index, weight);
	}

	// Renormalize
	float totalW = v.weight[0].getWeight() + v.weight[1].getWeight() + v.weight[2].getWeight() + v.weight[3].getWeight();
	for(int i=0; i<MAX_WEIGHTS; ++i)
		v.weight[i].setWeight(v.weight[i].getWeight() / totalW);
}

Body::Body(const char * meshfile, float scale) :
m_scale(scale)
{
	m_vb = 0;
	m_ib = 0;

	glGenBuffers(1, &m_vb);
	glGenBuffers(1, &m_ib);

	// Start the import on the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll t
	// probably to request more postprocessing than we do in this example.
	const aiScene* scene = aiImportFile(meshfile, 
	aiProcess_GenSmoothNormals		 |
	aiProcess_Triangulate            |
	aiProcess_JoinIdenticalVertices);

	// If the import failed, report it
	if(!scene)
	{
		printf("%s \n", aiGetErrorString());
		return;
	}
	
	// Get array allocation data
	unsigned int partCount = scene->mNumMeshes;
	unsigned int vertexCount = 0;
	unsigned int triangleCount = 0;
	unsigned int boneCount = 0;
	unsigned int materialCount = scene->mNumMaterials;

	m_material.reserve(materialCount);

	for(unsigned int i = 0; i < materialCount; ++i)
	{
		aiString aiDiff, aiNorm, aiHeig, aiSpec;

		scene->mMaterials[i]->GetTexture (aiTextureType_DIFFUSE, 0, &aiDiff);
		scene->mMaterials[i]->GetTexture (aiTextureType_NORMALS, 0, &aiNorm);
		scene->mMaterials[i]->GetTexture (aiTextureType_HEIGHT, 0, &aiHeig);
		scene->mMaterials[i]->GetTexture (aiTextureType_SPECULAR, 0, &aiSpec);

		std::string diff(aiDiff.C_Str()), norm(aiNorm.C_Str()), heig(aiHeig.C_Str()), spec(aiSpec.C_Str());

		diff = diff.substr(diff.rfind("/")+1);
		norm = norm.substr(norm.rfind("/")+1);
		heig = heig.substr(heig.rfind("/")+1);
		spec = spec.substr(spec.rfind("/")+1);

		m_material.push_back(sMaterial(diff.c_str(), norm.c_str(), heig.c_str(), spec.c_str()));

		/*
		printf("m_materialSH[%i]: d: %s, n: %s, h:%s, s:%s \n", i,
			m_material[i].diffuse.getStr(),
			m_material[i].normal.getStr(),
			m_material[i].height.getStr(),
			m_material[i].specular.getStr());
			*/
	}

	for (unsigned int i = 0; i<partCount; ++i) {
		m_partData.push_back(sPart());
		m_partData[i].offset = triangleCount;
		m_partData[i].count  = scene->mMeshes[i]->mNumFaces;

		vertexCount    += scene->mMeshes[i]->mNumVertices;
		triangleCount  += scene->mMeshes[i]->mNumFaces; //maybe
		boneCount	   += scene->mMeshes[i]->mNumBones;
	}

	m_vertexData.reserve(vertexCount);
	m_triangleData.reserve(triangleCount);
	m_boneOffset.reserve(boneCount);
	m_boneSH.reserve(boneCount);
	m_boneParent.reserve(boneCount);

	// Read mesh data
	aiMesh** mesh = scene->mMeshes;
	aiBone** bone;
	aiVector3D* vertex;
	aiVector3D* normal;
	aiVector3D** texCoord;
	aiFace* face;

	unsigned int vertexOffset = 0, triangleOffset = 0, boneOffset = 0;

	for(unsigned int i=0; i<scene->mNumMeshes; ++i) {
		vertex		= mesh[i]->mVertices;
		normal		= mesh[i]->mNormals;
		texCoord	= mesh[i]->mTextureCoords;
		face		= mesh[i]->mFaces;
		bone        = mesh[i]->mBones;

		// Get vertex positions
		for(unsigned int j=0; j<mesh[i]->mNumVertices; ++j) {
			m_vertexData.push_back(sVertex());
			m_vertexData[vertexOffset + j].position = glm::vec3(vertex[j].x,	vertex[j].y,	vertex[j].z) * scale;
			m_vertexData[vertexOffset + j].normal	= glm::vec3(normal[j].x,	normal[j].y,	normal[j].z);
			m_vertexData[vertexOffset + j].texCoord = glm::vec2(texCoord[0][j][0], texCoord[0][j][1]);
		}

		// Get triangle indices
		for(unsigned int j=0; j<mesh[i]->mNumFaces; ++j) {
			if(face[j].mNumIndices == 3) {
				m_triangleData.push_back(sTriangle());
				m_triangleData[triangleOffset+j].index[0] = vertexOffset + face[j].mIndices[0];
				m_triangleData[triangleOffset+j].index[1] = vertexOffset + face[j].mIndices[1];
				m_triangleData[triangleOffset+j].index[2] = vertexOffset + face[j].mIndices[2];
			} else {
				fprintf(stderr, "Faces with != 3 indices not implemented\n");
			}
		}

		// Get bones with weights
		for(unsigned int j=0; j<mesh[i]->mNumBones; ++j) {
			StringHash sh(bone[j]->mName.C_Str());
			int index = findStringHashInVector(sh, m_boneSH);
			if (index == -1)
			{
				// Bone does not exist! create it!
				//printf("New bone: '%s'", sh.getStr());

				index = (int)m_boneOffset.size();

				m_boneSH.push_back(sh);
				m_boneParent.push_back(-1);
				glm::mat4 M;
				copyAiMatrixToGLM(&bone[j]->mOffsetMatrix, M);
				glm::vec4 pos = glm::column(M, 3) * m_scale;
				M[3][0] = pos.x;
				M[3][1] = pos.y;
				M[3][2] = pos.z;
				
				m_boneOffset.push_back(Transform(M));
			}
			else
				continue;
			//	printf("Existing bone: '%s'", sh.getStr());

			//printf(", numWeights in bone: %i\n", bone[j]->mNumWeights);

			// Store vertex weights for this bone
			for(unsigned int k=0; k<bone[j]->mNumWeights; ++k) {
				const aiVertexWeight& vw = bone[j]->mWeights[k];
				sVertex & v = m_vertexData[vertexOffset+vw.mVertexId];
				insertWeight(v, index, vw.mWeight);
			}
		}

		vertexOffset   += mesh[i]->mNumVertices;
		triangleOffset += mesh[i]->mNumFaces;
		boneOffset     += mesh[i]->mNumBones;
	}

	//printf("Hierarchy: \n");
	//printNode(scene->mRootNode, 0);

	readBoneHierarchy(scene->mRootNode, -1);

	// print the local version of the hierarchy
	//for (unsigned int i = 0; i < (unsigned int)getBoneCount(); ++i)
	//	printf("[%u] %s : parent %i, dist to leaf: %i \n", i, m_boneSH[i].getStr(), m_boneParent[i], m_boneMaxDistToLeaf[i]);
	
	aiReleaseImport(scene);

	// If we have an animation named as the meshfile.md5anim
	addAnimation(meshfile, "default");

	fillBuffers();
}

Body::~Body()
{
	glDeleteBuffers(1, &m_vb);
	glDeleteBuffers(1, &m_ib);
}

Body::sPart * Body::getPart(const StringHash & sh)
{
	for (size_t i = 0; i < getPartCount(); ++i)
	{
		if (m_partSH[i] == sh)
			return &m_partData[i];
	}

	return nullptr;
}

Body::sPart * Body::getPart(size_t index)
{
	if (index < getPartCount())
		return &m_partData[index];

	return nullptr;
}

int	Body::getPartIndex(const StringHash & sh)
{
	return findStringHashInVector(sh, m_partSH);
}

Transform * Body::getBone(const StringHash & sh)
{
	for (size_t i = 0; i < getBoneCount(); ++i)
	{
		if (m_boneSH[i] == sh)
			return &m_boneOffset[i];
	}

	return nullptr;
}

Transform * Body::getBone(size_t index)
{
	if (index < getBoneCount())
		return &m_boneOffset[index];

	return nullptr;
}

int Body::getBoneIndex(const StringHash & sh)
{
	return findStringHashInVector(sh, m_boneSH);
}

Animation * Body::getAnimation(const StringHash & sh)
{
	for (size_t i = 0; i < getAnimationCount(); ++i)
	{
		if (m_animationSH[i] == sh)
			return &m_animationData[i];
	}
	
	return nullptr;
}

Animation * Body::getAnimation(size_t index)
{
	if (index < getAnimationCount())
		return &m_animationData[index];

	return nullptr;
}

int Body::getAnimationIndex(const StringHash & sh)
{
	return findStringHashInVector(sh, m_boneSH);
}

void Body::addAnimation(const char* animationfile, const char* name)
{
	const aiScene* scene = aiImportFile(animationfile, 0);

	// Exit if we have no animations in the file
	if (!scene || scene->mNumAnimations == 0)
		return;

	for(unsigned int i=0; i<scene->mNumAnimations; ++i) {
		unsigned int	numChannels		= scene->mAnimations[i]->mNumChannels;
		double			duration		= scene->mAnimations[i]->mDuration;
		double			ticksPerSecond	= scene->mAnimations[i]->mTicksPerSecond;

		printf("New animation: '%s', channels: %i, duration %.2f, tps: %.2f\n", name, numChannels, duration, ticksPerSecond);

		m_animationSH.push_back(StringHash(name));
		m_animationData.push_back(Animation(numChannels, (float) (duration/ticksPerSecond), (float)ticksPerSecond));

		Animation & anim = m_animationData[i];

		printf("%s\n", m_animationSH[i].getStr());
		for(unsigned int j=0; j<scene->mAnimations[i]->mNumChannels; ++j) {
			
			aiNodeAnim * channel = scene->mAnimations[i]->mChannels[j];
			unsigned int posKeys = channel->mNumPositionKeys;
			unsigned int rotKeys = channel->mNumRotationKeys;
			unsigned int sclKeys = channel->mNumScalingKeys;
			unsigned int maxKeys = glm::max(posKeys, glm::max(rotKeys, sclKeys));
			StringHash channel_sh(channel->mNodeName.C_Str());

			AnimationChannel ch(maxKeys);

			for(unsigned int k=0; k<maxKeys; ++k)
			{
				//float w = (float)k / (float)maxKeys;
				//int lowerFrame, upperFrame;

				const aiVector3D &		aPos = channel->mPositionKeys[k].mValue;
				const aiQuaternion &	aRot = channel->mRotationKeys[k].mValue;
				const aiVector3D &		aScl = channel->mScalingKeys[0].mValue;

				glm::vec3 pos = glm::vec3(aPos.x, aPos.y, aPos.z) * m_scale;
				glm::quat rot = glm::quat(aRot.w, aRot.x, aRot.y, aRot.z);
				glm::vec3 scl = glm::vec3(aScl.x, aScl.y, aScl.z);

				ch.m_pose.push_back(Transform(pos, rot, scl));
			}
			int index = getBoneIndex(channel_sh);
			if(index > -1)
				anim.setChannel((size_t)index, ch);
		}
	}
}

void Body::readBoneHierarchy(aiNode* node, int parent)
{
	if (!node)
		return;

	StringHash sh(node->mName.C_Str());
	int index = getBoneIndex(sh);
	if (index > -1)
	{
		m_boneParent[index] = parent;
		parent = index;
	}

	for (size_t i = 0; i < node->mNumChildren; ++i)
		readBoneHierarchy(node->mChildren[i], parent);
}

void Body::fillBuffers()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_vb);
	glBufferData(GL_ARRAY_BUFFER, getVertexCount() * sizeof(sVertex), &getVertexData()[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, getTriangleCount() * sizeof(sTriangle), &getTriangleData()[0], GL_STATIC_DRAW);
}
