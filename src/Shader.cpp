#include "Shader.h"
#include <cstdio>
#include <cstdlib>

Shader * Shader::s_boundShader = NULL;

/**
 * readTextFile - reads a simple textfile specified by filename,
 * if everything is ok, a pointer to a null-terminated string is returned,
 * otherwise NULL is returned.
 */

char* readTextFile(const char *filename) {
	FILE * file;
#ifdef _WIN32
	fopen_s(&file, filename, "r");
#else
    FILE *file = fopen(filename, "r");
#endif
    if(file == NULL)
    {
    	printf("Cannot open file %s", filename);
    	return 0;
    }

    fseek(file, 0, SEEK_END);

    char *buffer = NULL;
    int bytesinfile = ftell(file);
    rewind(file);

    if(bytesinfile > 0)
    {
        buffer = (char*)malloc(bytesinfile+1);
        size_t bytesread = fread( buffer, 1, bytesinfile, file);
        buffer[bytesread] = '\0'; // Terminate the string with a null character
    }

    fclose(file);

    return buffer;
}

void printShaderInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;
 
    glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);
 
    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
        printf("%s\n",infoLog);
        free(infoLog);
    }
}
 
void printProgramInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;
 
    glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);
 
    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
        printf("%s\n",infoLog);
        free(infoLog);
    }
}

Shader::Shader()
{
	m_program = glCreateProgram();
	m_compiled = false;
}

Shader::~Shader()
{
    glDeleteProgram(m_program);
}

void Shader::attachShader(GLenum shaderType, const char * filename)
{
	GLuint shader = glCreateShader(shaderType);
	char * txt = readTextFile(filename);
	const char * ctxt = txt;

	glShaderSource(shader, 1, &ctxt, NULL);
	glCompileShader(shader);

	if(txt)
		free(txt);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	// kolla om det sket sig
	if(status != GL_TRUE)
	{
		printf("Shader compile error: \n");
		printShaderInfoLog(shader);

		glDeleteShader(shader);
		return;
	}

	glAttachShader(m_program, shader);
	glDeleteShader(shader);
}

void Shader::build()
{
	glLinkProgram(m_program);

	GLint status;
	glGetProgramiv(m_program, GL_VALIDATE_STATUS, &status);

	if(status != GL_TRUE)
	{
		printf("Program validate error: \n");
		printProgramInfoLog(m_program);
	}

	m_compiled = true;
}

void Shader::bindAttribLocation(int location, const char * attrib)
{
	glBindAttribLocation(m_program, location, attrib);
}

void Shader::bindFragDataLocation(int location, const char * fragData)
{
	glBindFragDataLocation(m_program, location, fragData);
}

void Shader::bind()
{
    if(!m_compiled)
        return;

    glUseProgram(m_program);
    s_boundShader = this;
}

void Shader::unbind()
{
	glUseProgram(0);
	s_boundShader = NULL;
}