#include "Body.h"

bool ImportScene( const aiScene* scene)
{
	return true;
}

Body::Body(const char * meshfile, const char * animfile)
{
	m_vertexData 	= NULL;
	m_vertexCount 	= 0;

	m_triangleData	= NULL;
	m_triangleCount = 0;

	// Start the import on the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll t
	// probably to request more postprocessing than we do in this example.
	const aiScene* scene = aiImportFile(meshfile, 
	aiProcess_CalcTangentSpace       | 
	aiProcess_Triangulate            |
	aiProcess_JoinIdenticalVertices  |
	aiProcess_SortByPType);
	
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
	aiVertexWeight** weight;
	for(unsigned int i=0; i<scene->mNumMeshes; ++i) {
		vertex = mesh[i]->mVertices;
		//bone = mesh[i]->mBones;
		m_vertexCount += mesh[i]->mNumVertices;

		for(unsigned int j=0; j<mesh[i]->mNumVertices; ++j) {
			m_vertexData[m_vertexCount+j].position.x = vertex[j][0];
			m_vertexData[m_vertexCount+j].position.y = vertex[j][1];
			m_vertexData[m_vertexCount+j].position.z = vertex[j][2];
		}
	}
	

	// If we have a separate animation file
	if(animfile != NULL) {
		scene = aiImportFile(animfile, 
		aiProcess_CalcTangentSpace       | 
		aiProcess_Triangulate            |
		aiProcess_JoinIdenticalVertices  |
		aiProcess_SortByPType);

	}

	// If the import failed, report it
	if(!scene)
	{
		printf("%s \n", aiGetErrorString());
		return;
	}

	aiReleaseImport(scene);



}

Body::~Body()
{
	if(m_vertexData)
		delete[] m_vertexData;

	if(m_triangleData)
		delete[] m_triangleData;
}
