#pragma once

#include <GL/glew.h>

class Shader
{
public:
	static Shader* getBoundShader() { return s_boundShader; }

	Shader();
	~Shader();

	void attachShader(GLenum shaderType, const char * filename);

	void link();
	void validate();

	void bindAttribLocation(int location, const char * attrib);
	void bindFragDataLocation(int location, const char * fragData);

	void bind();
	void unbind();

	int getUniformLocation(const char * uniform) const;
	int getAttribLocation(const char * attribute) const;

	GLuint getProgram() const { return m_program; }

private:
	static Shader * s_boundShader;
	GLuint m_program;

	bool m_compiled;
};