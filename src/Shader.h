#pragma once

#include <GL/glew.h>

class Shader
{
public:
	static Shader * getBoundShader();

	Shader();
	~Shader();

	void attachShader(GLenum shaderType, const char * filename);

	void link();

	void bindAttribLocation(int location, const char * attrib);
	void bindFragDataLocation(int location, const char * fragData);

	void bind();
	void unbind();

	int getUniformLocation(const char * uniform);
	int getAttribLocation(const char * attribute);

	GLuint getProgram() { return m_program; }

private:
	static Shader * s_boundShader;
	GLuint m_program;

	bool m_compiled;
};