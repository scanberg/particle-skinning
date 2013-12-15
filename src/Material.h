#pragma once

#include <glm/glm.hpp>
#include "Texture2D.h"
#include "Shader.h"
//#include <FreeImage.h>

class Material
{
public:
	enum TYPE_TEXTURE
	{
		TEX_DIFFUSE,
		TEX_NORMAL,
		TEX_HEIGHT,
		TEX_SMOOTHNESS,

		TEX_COUNT = 4
	};

	enum TYPE_ATTRIBUTE
	{
		ATTR_DIFFUSE,
		ATTR_HEIGHT,
		ATTR_SMOOTHNESS,
		ATTR_METALLIC,

		ATTR_COUNT = 4
	};

	Material(Shader& shader);

	void bind();

	void setTexture(TYPE_TEXTURE tex, Texture2D* ptr) { m_texture[tex] = ptr; }
	void setTextureUniform(TYPE_TEXTURE tex, const char * uniform);
	void setTextureUniform(TYPE_TEXTURE tex, GLint location) { m_texLocation[tex] = location; }

	void setAttribute(TYPE_ATTRIBUTE attr, const glm::vec3 &val) { m_attribute[attr] = val; }
	void setAttributeUniform(TYPE_ATTRIBUTE attr, const char * uniform);
	void setAttributeUniform(TYPE_ATTRIBUTE attr, GLint location) { m_attrLocation[attr] = location; }

	bool hasTexture(TYPE_TEXTURE tex) const { return m_texture[tex] != nullptr; }

	Texture2D*	getTexture(TYPE_TEXTURE tex) const { return m_texture[tex]; }
	GLint		getTextureUniform(TYPE_TEXTURE tex) const { return m_texLocation[tex]; }

	glm::vec3	getAttribute(TYPE_ATTRIBUTE attr) const { return m_attribute[attr]; }
	GLint		getAttributeUniform(TYPE_TEXTURE attr) const { return m_attrLocation[attr]; }

	Shader		getShader() const { return m_shader; }

private:
	Texture2D*	m_texture[TEX_COUNT];
	glm::vec3	m_attribute[ATTR_COUNT];

	GLint		m_texLocation[TEX_COUNT];
	GLint		m_attrLocation[ATTR_COUNT];

	Shader&		m_shader;
};
