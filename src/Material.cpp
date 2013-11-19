#include "Material.h"

Material::Material(const char* imfile)
{
	/*
	FREE_IMAGE_FORMAT format = FreeImage_GetFileType(imfile);
	FIBITMAP* img = FreeImage_Load(format, imfile);
	unsigned int height, width;
	height = FreeImage_GetHeight(img);
	width  = FreeImage_GetWidth(img);

	mDiffuse = (char*)FreeImage_GetBits(img);
	subscribers = 1;

	*/

	//printf("Antal %i \n", (int) scene->mMaterials[0]->GetTextureCount(aiTextureType_DIFFUSE));
	//aiString tgafile;
	//scene->mMaterials[0]->GetTexture(aiTextureType_DIFFUSE, 0, &tgafile);
	//printf("Texture path: %s \n", tgafile.C_Str());
}
Material::~Material()
{
	if (subscribers == 1 && mDiffuse != NULL)
		delete[] mDiffuse;
	else
		subscribers -= 1;
}

char*& Material::subscribe()
{
	subscribers += 1;
	return mDiffuse;
}

void Material::unsubscribe()
{
	subscribers -= 1;
	if (subscribers == 0)
		delete this;
}
