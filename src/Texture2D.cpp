#include <cstdlib>
#include <cstdio>
#include "Texture2D.h"

// http://stackoverflow.com/questions/7046270/loading-a-tga-file-and-using-it-with-opengl
typedef struct
{
    unsigned char imageTypeCode;
    short int imageWidth;
    short int imageHeight;
    unsigned char bitCount;
    unsigned char *imageData;
} TGAFILE;

bool LoadTGAFile(const char *filename, TGAFILE *tgaFile)
{
    FILE *filePtr;
    unsigned char ucharBad;
    short int sintBad;
    long imageSize;
    int colorMode;
    unsigned char colorSwap;

    // Open the TGA file.
    filePtr = fopen(filename, "rb");
    if (filePtr == NULL)
    {
        return false;
    }

    // Read the two first bytes we don't need.
    fread(&ucharBad, sizeof(unsigned char), 1, filePtr);
    fread(&ucharBad, sizeof(unsigned char), 1, filePtr);

    // Which type of image gets stored in imageTypeCode.
    fread(&tgaFile->imageTypeCode, sizeof(unsigned char), 1, filePtr);

    // For our purposes, the type code should be 2 (uncompressed RGB image)
    // or 3 (uncompressed black-and-white images).
    if (tgaFile->imageTypeCode != 2 && tgaFile->imageTypeCode != 3)
    {
        fclose(filePtr);
        return false;
    }

    // Read 13 bytes of data we don't need.
    fread(&sintBad, sizeof(short int), 1, filePtr);
    fread(&sintBad, sizeof(short int), 1, filePtr);
    fread(&ucharBad, sizeof(unsigned char), 1, filePtr);
    fread(&sintBad, sizeof(short int), 1, filePtr);
    fread(&sintBad, sizeof(short int), 1, filePtr);

    // Read the image's width and height.
    fread(&tgaFile->imageWidth, sizeof(short int), 1, filePtr);
    fread(&tgaFile->imageHeight, sizeof(short int), 1, filePtr);

    // Read the bit depth.
    fread(&tgaFile->bitCount, sizeof(unsigned char), 1, filePtr);

    // Read one byte of data we don't need.
    fread(&ucharBad, sizeof(unsigned char), 1, filePtr);

    // Color mode -> 3 = BGR, 4 = BGRA.
    colorMode = tgaFile->bitCount / 8;
    imageSize = tgaFile->imageWidth * tgaFile->imageHeight * colorMode;

    // Allocate memory for the image data.
    tgaFile->imageData = (unsigned char*)malloc(sizeof(unsigned char)*imageSize);

    // Read the image data.
    fread(tgaFile->imageData, sizeof(unsigned char), imageSize, filePtr);

    // Change from BGR to RGB so OpenGL can read the image data.
    for (int imageIdx = 0; imageIdx < imageSize; imageIdx += colorMode)
    {
        colorSwap = tgaFile->imageData[imageIdx];
        tgaFile->imageData[imageIdx] = tgaFile->imageData[imageIdx + 2];
        tgaFile->imageData[imageIdx + 2] = colorSwap;
    }

    fclose(filePtr);
    return true;
}

Texture2D::Texture2D(const char * filename)
{
	TGAFILE tga;
	if(!LoadTGAFile(filename, &tga))
	{
		printf("Failed to load file %s as tga.\n",filename);
		return;
	}

	printf("Loaded: '%s', w: %i, h: %i, bpp: %i \n", filename, tga.imageWidth, tga.imageHeight, tga.bitCount);

	GLint format;
	GLint internalFormat;
	switch(tga.bitCount)
	{
		case(8):
			format = GL_RED;
			internalFormat = GL_R8;
			break;
		case(16):
			format = GL_RG;
			internalFormat = GL_RG8;
			break;
		case(24):
			format = GL_RGB;
			internalFormat = GL_RGB8;
			break;
		case(32):
			format = GL_RGBA;
			internalFormat = GL_RGBA8;
			break;
		default:
			printf("error, bpp not recognized");
			return;
	}

	initData(tga.imageWidth, tga.imageHeight, tga.imageData, internalFormat, format, GL_UNSIGNED_BYTE, GL_TRUE);
}

Texture2D::Texture2D(GLsizei width, GLsizei height, const GLvoid * data,
	GLint internalFormat, GLenum format, GLenum type, GLboolean mipmap)
{
	initData(width, height, data, internalFormat, format, type, mipmap);
}

Texture2D::~Texture2D()
{
	if(glIsTexture(m_textureID))
		glDeleteTextures(1, &m_textureID);
}

GLuint Texture2D::getID() const
{
	return m_textureID;
}

GLsizei Texture2D::getWidth() const
{
	return m_width;
}

GLsizei Texture2D::getHeight() const
{
	return m_height;
}

void Texture2D::bind(GLuint channel) const
{
	glActiveTexture(GL_TEXTURE0 + channel);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
}

void Texture2D::generateMipmap()
{
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture2D::setParameter(GLenum name, GLint param)
{
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	glTexParameteri(GL_TEXTURE_2D, name, param);
}

void Texture2D::setParameter(GLenum name, GLfloat param)
{
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	glTexParameterf(GL_TEXTURE_2D, name, param);
}

void Texture2D::setData(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
	const GLvoid * data, GLenum format, GLenum type)
{
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	glTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset,
 		width, height, format, type, data);
}

void Texture2D::initData(GLsizei width, GLsizei height, const GLvoid * data,
	GLint internalFormat, GLenum format,
	GLenum type, GLboolean mipmap)
{
	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, data);
	if(mipmap)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	m_format = format;
	m_internalFormat = internalFormat;
	m_type = type;
	m_width = width;
	m_height = height;
}