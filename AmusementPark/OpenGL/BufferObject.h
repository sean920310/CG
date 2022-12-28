#pragma once
#include <glad\glad.h>

#define MAX_FBO_TEXTURE_AMOUNT 4

class VBO
{
public:
	GLuint id;
	VBO(GLfloat* vertices, GLsizeiptr size)
	{
		//�ͦ�unique ID��VBO ������l��
		glGenBuffers(1, &id);
		//�NBuffer�j����wVBO
		glBindBuffer(GL_ARRAY_BUFFER, id);
		//��ƾڽƻs��Buffer�� �۷��ƻs��ثe�j�w��VBO
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	void Bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, id);
	}
	void Unbind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	void Delete()
	{
		glDeleteBuffers(1, &id);
	}
};

class VAO
{
public:
	GLuint id;
	VAO()
	{
		glGenVertexArrays(1, &id);
	}

	void LinkAttrib(VBO& vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset)
	{
		vbo.Bind();
		//�i�DShader����x�s���覡�O�ƻ�
		glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
		glEnableVertexAttribArray(layout);
		vbo.Unbind();
	}
	void Bind()
	{
		glBindVertexArray(id);
	}
	void Unbind()
	{
		glBindVertexArray(0);
	}
	void Delete()
	{
		glDeleteVertexArrays(1, &id);
	}
};

class EBO
{
public:
	GLuint id;
	EBO(GLuint* indices, GLsizeiptr size)
	{
		glGenBuffers(1, &id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
	}

	void Bind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	}
	void Unbind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	void Delete()
	{
		glDeleteBuffers(1, &id);
	}
};

struct UBO
{
	GLuint ubo;
	GLsizeiptr size;
};
struct FBO
{
	GLuint fbo;	//frame buffer
	GLuint textures[MAX_FBO_TEXTURE_AMOUNT];	//attach to color buffer
	GLuint rbo;	//attach to depth and stencil
};