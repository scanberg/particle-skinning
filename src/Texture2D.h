#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifndef NULL
#define NULL 0
#endif

class Texture2D
{
public:
	Texture2D(GLsizei width, GLsizei height, const GLvoid * data=NULL,
		GLint internalFormat=GL_RGBA8, GLenum format=GL_RGB,
		GLenum type=GL_UNSIGNED_BYTE, GLboolean mipmap=GL_FALSE);
	~Texture2D();

	GLuint getID() const;
	GLsizei getWidth() const;
	GLsizei getHeight() const;

	void bind(GLuint channel = GL_TEXTURE0) const;
	void generateMipmap();
	void setParameter(GLenum name, GLint param);
	void setParameter(GLenum name, GLfloat param);

	void setData(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
		const GLvoid * data, GLenum format, GLenum type);
private:
	GLuint	m_textureID;
	GLsizei m_width, m_height;
	GLint	m_internalFormat;
	GLenum	m_format;
	GLenum	m_type;
};