#include <glm/gtc/type_ptr.hpp>
#include "Material.h"

const char * textureUniformDefaults[] = {
	"texture_diffuse",
	"texture_normal",
	"texture_height",
	"texture_smoothness"};

const char * attributeUniformDefaults[] = {
	"material_diffuse",
	"material_height",
	"material_metallic",
	"material_smoothness"};

Material::Material(Shader& shader) :
m_shader(shader)
{
	for(int i=0; i<TEX_COUNT; ++i)
	{
		m_texture[i] = nullptr;
		m_texLocation[i] = m_shader.getUniformLocation(textureUniformDefaults[i]);
	}

	m_attribute[ATTR_DIFFUSE]		= glm::vec3(1);
	m_attribute[ATTR_HEIGHT]		= glm::vec3(0.03);
	m_attribute[ATTR_METALLIC]		= glm::vec3(0);
	m_attribute[ATTR_SMOOTHNESS]	= glm::vec3(0);

	for(int i=0; i<ATTR_COUNT; ++i)
		m_attrLocation[i] = m_shader.getUniformLocation(attributeUniformDefaults[i]);
}

void Material::bind()
{
	m_shader.bind();

	for(int i=0; i<TEX_COUNT; ++i)
	{
		if( hasTexture(static_cast<TYPE_TEXTURE>(i)) )
		{
			glUniform1i(m_texLocation[i], i);
			m_texture[i]->bind(i);
		}
	}

	for(int i=0; i<ATTR_COUNT; ++i)
	{
		switch(i)
		{
		case ATTR_DIFFUSE:
			glUniform3fv(m_attrLocation[i], 1, glm::value_ptr(m_attribute[i]));
			break;
		case ATTR_HEIGHT:
		case ATTR_SMOOTHNESS:
		case ATTR_METALLIC:
			glUniform1f(m_attrLocation[i], m_attribute[i][0]);
			break;
		default:
			break;
		}
	}
}