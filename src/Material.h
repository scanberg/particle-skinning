#pragma once

#include <glm/glm.hpp>
#include <FreeImage.h>

class Material
{
public:
	Material(const char* imfile = NULL);
	~Material();

	void bind();
	char*& subscribe();
	void unsubscribe();

private:
	char* mDiffuse;
	//Texture2D* mSpecular;
	//Texture2D* mAmbient;
	//Texture2D* mNormal;
	unsigned int* subscribers;
};
