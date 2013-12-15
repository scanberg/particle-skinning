#include <GL/glew.h>

class VertexArray
{
public:
	VertexArray() { glGenVertexArrays(1, &m_id); }
	~VertexArray() { glDeleteVertexArrays(1, &m_id); }

	void bind() const { glBindVertexArray(m_id); }
	void unbind() const { glBindVertexArray(0); }

	GLuint getId() const { return m_id; }
private:
	GLuint m_id;
};

class VertexBuffer
{
public:
	VertexBuffer() { glGenBuffers(1, &m_id); }
	~VertexBuffer() { glDeleteBuffers(1, &m_id); }

	void bind() const { glBindBuffer(GL_ARRAY_BUFFER, m_id); }
	void unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }

	GLuint getId() const { return m_id; }
private:
	GLuint m_id;
};

class IndexBuffer
{
public:
	IndexBuffer() { glGenBuffers(1, &m_id); }
	~IndexBuffer() { glDeleteBuffers(1, &m_id); }

	void bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id); }
	void unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

	GLuint getId() const { return m_id; }
private:
	GLuint m_id;
};