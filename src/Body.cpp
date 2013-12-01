#include "Body.h"

#include <GL/glew.h>
#include <assimp/cimport.h>        // Plain-C interface
#include <assimp/scene.h>          // Output data structure
#include <assimp/postprocess.h>    // Post processing flags
#include <glm/gtc/type_ptr.hpp>    // aiMatrix4 -> glm:mat4

Body::Body(const char * meshfile)
{
	m_vertexData 		= NULL;
	m_vertexCount 		= 0;

	m_triangleData		= NULL;
	m_triangleCount 	= 0;

	m_partSH			= NULL;
	m_partData 			= NULL;
	m_partCount 		= 0;

	m_boneSH 			= NULL;
	m_boneData			= NULL;
	m_boneCount 		= 0;

	m_animationCount 	= 0;

	m_vb = 0;
	m_ib = 0;
	m_va = 0;

	glGenBuffers(1, &m_vb);
	glGenBuffers(1, &m_ib);
    glGenVertexArrays(1, &m_va);

	// Start the import on the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll t
	// probably to request more postprocessing than we do in this example.
	const aiScene* scene = aiImportFile(meshfile, 
	aiProcess_GenSmoothNormals		 |
	aiProcess_CalcTangentSpace       | 
	aiProcess_Triangulate            |
	aiProcess_JoinIdenticalVertices  |
	aiProcess_SortByPType);

	// If the import failed, report it
	if(!scene)
	{
		printf("%s \n", aiGetErrorString());
		return;
	}
	
	// Get array allocation data
	m_partCount = scene->mNumMeshes;
	m_partData  = new sPart[m_partCount];

	for(unsigned int i=0; i<scene->mNumMeshes; ++i) {
		m_partData[i].offset = m_vertexCount;
		m_partData[i].count  = scene->mMeshes[i]->mNumVertices;

		m_vertexCount    += scene->mMeshes[i]->mNumVertices;
		m_triangleCount  += scene->mMeshes[i]->mNumFaces; //maybe
		m_boneCount	     += scene->mMeshes[i]->mNumBones;
		m_animationCount += scene->mNumAnimations;
	}

	m_vertexData    = new sVertex[m_vertexCount];
	m_triangleData  = new sTriangle[m_triangleCount];
	m_boneData      = new Transform[m_boneCount];
	//m_animationData = new Animation[m_animationCount];
	m_boneSH        = (StringHash*) malloc(sizeof(StringHash)*m_boneCount);

	// Read mesh data
	aiMesh** mesh = scene->mMeshes;
	aiBone** bone;
	aiVector3D* vertex;
	aiVector3D* normal;
	aiVector3D** texCoord;
	aiFace* face;
	aiVertexWeight** weight;

	unsigned int vertexOffset = 0, triangleOffset = 0, boneOffset = 0;

	for(unsigned int i=0; i<scene->mNumMeshes; ++i) {
		vertex		= mesh[i]->mVertices;
		normal		= mesh[i]->mNormals;
		texCoord	= mesh[i]->mTextureCoords;
		face		= mesh[i]->mFaces;
		bone        = mesh[i]->mBones;

		// Get vertex positions
		for(unsigned int j=0; j<mesh[i]->mNumVertices; ++j) {
			m_vertexData[vertexOffset + j].position = glm::vec3(vertex[j][0],	vertex[j][1],	vertex[j][2]);
			m_vertexData[vertexOffset + j].normal	= glm::vec3(normal[j][0],	normal[j][1],	normal[j][2]);
			//m_vertexData[vertexOffset + j].texCoord = glm::vec2(texCoord[j][0][0], texCoord[j][0][1]);
		}

		// Get triangle indices
		for(unsigned int j=0; j<mesh[i]->mNumFaces; ++j) {
			if(face[j].mNumIndices == 3) {
				m_triangleData[triangleOffset+j].index[0] = vertexOffset + face[j].mIndices[0];
				m_triangleData[triangleOffset+j].index[1] = vertexOffset + face[j].mIndices[1];
				m_triangleData[triangleOffset+j].index[2] = vertexOffset + face[j].mIndices[2];
			} else {
				fprintf(stderr, "Faces with != 3 indices not implemented\n");
			}
		}
		// Get bones
		for(unsigned int j=0; j<mesh[i]->mNumBones; ++j) {
			m_boneSH[boneOffset+j]   = StringHash(bone[j]->mName.C_Str());
			m_boneData[boneOffset+j] = Transform(glm::make_mat4( (float*)(bone[j]->mOffsetMatrix[0])) );

			for(unsigned int k=0; k<bone[j]->mNumWeights; ++k) {
				const aiVertexWeight& vw = bone[j]->mWeights[k];

				for(unsigned int l=0; l<4; ++l) {
					sVertex & v = m_vertexData[vertexOffset+vw.mVertexId];
					if(v.index[l] != -1) {
						v.index[l]  = boneOffset+j;
						v.weight[l] = vw.mWeight;
						break;
					}
				}
			}
		}

		vertexOffset   += mesh[i]->mNumVertices;
		triangleOffset += mesh[i]->mNumFaces;
		boneOffset     += mesh[i]->mNumBones;
	}
	
	// If we have a separate animation file
	addAnimation(meshfile, "default");

	aiReleaseImport(scene);

	fillBuffers();
}

void Body::addAnimation(const char* animationfile, const char* name)
{
	const aiScene* scene = aiImportFile(animationfile, 0);

	// Exit if we have no animations in the file
	if (scene->mNumAnimations == 0)
		return;

	m_animationCount += scene->mNumAnimations;

	for(unsigned int i=0; i<scene->mNumAnimations; ++i) {
		printf("Name of animation: \"%s\"\n", name);
		m_animationSH.push_back(StringHash(name));
		m_animationData.push_back(Animation(scene->mAnimations[i]->mNumChannels));

		Animation * anim = &m_animationData[i];

		printf("%s\n", m_animationSH[i].getStr());
		for(unsigned int j=0; j<scene->mAnimations[i]->mNumChannels; ++j) {

			aiNodeAnim * channel = scene->mAnimations[i]->mChannels[j];
			unsigned int posKeys = channel->mNumPositionKeys;
			unsigned int rotKeys = channel->mNumRotationKeys;
			unsigned int sclKeys = channel->mNumScalingKeys;
			unsigned int maxKeys = glm::max(posKeys, glm::max(rotKeys, sclKeys));

			AnimationChannel ch(maxKeys);

			printf("%d %d %d\n", posKeys, rotKeys, sclKeys);

			bool iplPos = posKeys < maxKeys;
			bool iplRot = rotKeys < maxKeys;
			bool iplScl = sclKeys < maxKeys;

			for(unsigned int k=0; k<maxKeys; ++k)
			{
				float w = (float)k / (float)maxKeys;
				int lowerFrame, upperFrame;
				glm::vec3 pos;
				glm::quat rot;
				glm::vec3 scl;

				if(iplPos) {
					lowerFrame = (int) (w * posKeys);
					upperFrame = lowerFrame + 1;
					glm::vec3 lowerPos = glm::make_vec3(&channel->mPositionKeys[lowerFrame].mValue.x);
					glm::vec3 upperPos = glm::make_vec3(&channel->mPositionKeys[upperFrame].mValue.x);
					pos = glm::mix(lowerPos, upperPos, w);
				} else {
					pos = glm::make_vec3(&channel->mPositionKeys[k].mValue.x);
				}

				if(iplRot) {
					lowerFrame = (int) (w * rotKeys);
					upperFrame = lowerFrame + 1;
					glm::quat lowerRot = glm::make_quat(&channel->mRotationKeys[lowerFrame].mValue.x);
					glm::quat upperRot = glm::make_quat(&channel->mRotationKeys[upperFrame].mValue.x);
					rot = glm::slerp(lowerRot, upperRot, w);
				} else {
					rot = glm::make_quat(&channel->mRotationKeys[k].mValue.x);
				}

				if(iplScl) {
					if(sclKeys == 1) {
						scl = glm::make_vec3(&channel->mScalingKeys[0].mValue.x);
					}
					else
					{
						lowerFrame = (int) (w * sclKeys);
						upperFrame = lowerFrame + 1;
						glm::vec3 lowerScl = glm::make_vec3(&channel->mScalingKeys[lowerFrame].mValue.x);
						glm::vec3 upperScl = glm::make_vec3(&channel->mScalingKeys[upperFrame].mValue.x);
						scl = glm::mix(lowerScl, upperScl, w);
					}
				} else {
					scl = glm::make_vec3(&channel->mScalingKeys[k].mValue.x);
				}

				ch.m_pose[k] = Transform(pos, rot, scl);

				//printf("P: %.2f %.2f %.2f, R: %.2f %.2f %.2f %.2f, S: %.2f %.2f %.2f\n",
				//	pos.x, pos.y, pos.z, rot.x, rot.y, rot.z, rot.w, scl.x, scl.y, scl.z);

			}
			anim->addChannel(ch);
		}
	}
}

Body::~Body()
{
	if(m_vertexData)
		delete[] m_vertexData;

	if(m_triangleData)
		delete[] m_triangleData;

	if(m_partData)
		delete[] m_partData;

	if(m_partSH)
		delete[] m_partSH;

	if(m_boneData)
		delete[] m_boneData;

	if(m_boneSH)
		delete[] m_boneSH;

	glDeleteBuffers(1, &m_vb);
	glDeleteBuffers(1, &m_ib);
	glDeleteVertexArrays(1, &m_va);
}

void Body::fillBuffers()
{
    glBindVertexArray(m_va);
    
	glBindBuffer(GL_ARRAY_BUFFER, m_vb);
	glBufferData(GL_ARRAY_BUFFER, m_vertexCount * sizeof(sVertex), m_vertexData, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_triangleCount * sizeof(sTriangle), m_triangleData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Body::sVertex), (const GLvoid*)0);

	glBindVertexArray(0);
}

Body::sPart * Body::getPart(const StringHash & sh){ return NULL; }
Transform * Body::getBone(const StringHash & sh){return NULL;}
Animation * Body::getAnimation(const StringHash & sh){return NULL;}

void Body::draw()
{
	glBindVertexArray(m_va);
   	glDrawElements(GL_TRIANGLES, 3 * getTriangleCount(), GL_UNSIGNED_INT, 0);
   	glBindVertexArray(0);
}

void Body::drawPart(unsigned int index)
{

}