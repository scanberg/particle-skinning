#include "Texture2D.h"

Texture2D::Texture2D(GLsizei width, GLsizei height, const GLvoid * data,
	GLint internalFormat, GLenum format, GLenum type, GLboolean mipmap)
{
	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, data);
	if(mipmap)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	m_width = width;
	m_height = height;
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
	glActiveTexture(channel);
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