#include "Body.h"
#include <GL/glew.h>

bool ImportScene( const aiScene* scene)
{
	return true;
}

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

	glGenBuffers(1, &m_vb);
	glGenBuffers(1, &m_ib);

	// Start the import on the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll t
	// probably to request more postprocessing than we do in this example.
	const aiScene* scene = aiImportFile(meshfile, 
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

	// Read mesh data
	aiMesh** mesh = scene->mMeshes;
	//aiBone** bone;
	aiVector3D* vertex;
	aiFace* face;
	aiVertexWeight** weight;

	unsigned int vertexOffset = 0, triangleOffset = 0;

	for(unsigned int i=0; i<scene->mNumMeshes; ++i) {
		vertex = mesh[i]->mVertices;
		face   = mesh[i]->mFaces;
		//bone = mesh[i]->mBones;

		// Get vertex positions
		for(unsigned int j=0; j<mesh[i]->mNumVertices; ++j) {
			m_vertexData[vertexOffset+j].position.x = vertex[j][0];
			m_vertexData[vertexOffset+j].position.y = vertex[j][1];
			m_vertexData[vertexOffset+j].position.z = vertex[j][2];
		}

		// Get triangle indices
		for(unsigned int j=0; j<mesh[i]->mNumFaces; ++j) {
			if(face[j].mNumIndices == 3) {
				m_triangleData[triangleOffset+j].index[0] = vertexOffset + face[i].mIndices[0];
				m_triangleData[triangleOffset+j].index[1] = vertexOffset + face[i].mIndices[1];
				m_triangleData[triangleOffset+j].index[2] = vertexOffset + face[i].mIndices[2];
			} else {
				fprintf(stderr, "Faces with != 3 indices not implemented\n");
			}
		}

		vertexOffset   += mesh[i]->mNumVertices;
		triangleOffset += mesh[i]->mNumFaces;
	}
	

	// If we have a separate animation file
	if(animfile != NULL) {
		scene = aiImportFile(animfile,
		aiProcess_CalcTangentSpace       | 
		aiProcess_Triangulate            |
		aiProcess_JoinIdenticalVertices  |
		aiProcess_SortByPType);

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
}

void Body::fillBuffers()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_vb);
	glBufferData(GL_ARRAY_BUFFER, m_vertexCount * sizeof(sVertex), m_vertexData, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_triangleCount * sizeof(sTriangle), m_triangleData, GL_STATIC_DRAW);
}
