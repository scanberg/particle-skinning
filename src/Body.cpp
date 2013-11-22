#include "Body.h"

#include <GL/glew.h>
#include <assimp/cimport.h>        // Plain-C interface
#include <assimp/scene.h>          // Output data structure
#include <assimp/postprocess.h>    // Post processing flags
#include <glm/gtc/type_ptr.hpp>    // aiMatrix4 -> glm:mat4

Body::Body(const char * meshfile, const char * animfile)
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

	m_animationSH 		= NULL;
	m_animationData 	= NULL;
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
	m_animationCount = scene->mNumAnimations;
	m_animationData  = (Animation*) malloc(sizeof(Animation)*m_animationCount);
	m_animationSH    = (StringHash*) malloc(sizeof(StringHash)*m_animationCount);

	for(unsigned int i=0; i<scene->mNumAnimations; ++i) {
		//printf("%s\n", scene->mAnimations[i]->mName.C_Str());
		m_animationSH[i] = StringHash(scene->mAnimations[i]->mName.C_Str());
		m_animationData[i] = Animation(scene->mAnimations[i]->mNumChannels,(unsigned int)scene->mAnimations[i]->mTicksPerSecond);
		for(unsigned int j=0; j<scene->mAnimations[i]->mNumChannels; ++j) {
			printf("%d %d %d\n", scene->mAnimations[i]->mChannels[j]->mNumPositionKeys, scene->mAnimations[i]->mChannels[j]->mNumRotationKeys, scene->mAnimations[i]->mChannels[j]->mNumScalingKeys);
			//for(unsigned int k=0; k<scene->mAnimations[i]->mChannels[j]->; ++j) {
			//glm::vec3 poskey = glm::
			//m_animationData[i].getChannel(j)->m_pose.setTranslation( scene->mAnimations[i]->mChannels[j].mPositionKeys );
			//m_animationData[i].m_pose.setOrientation( scene->mAnimations[i]->mChannels[j].mRotationKeys );
			//m_animationData[i].m_pose.setScale(       scene->mAnimations[i]->mChannels[j].mScalingKeys );
		}

	}

	aiReleaseImport(scene);

	fillBuffers();
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

	if(m_animationData)
		delete[] m_animationData;

	if(m_animationSH)
		delete[] m_animationSH;

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
