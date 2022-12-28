#pragma once
#include <glad\glad.h>

#define MAX_FBO_TEXTURE_AMOUNT 4

class VBO
{
public:
	GLuint id;
	VBO(GLfloat* vertices, GLsizeiptr size)
	{
		//生成unique ID給VBO 類似初始化
		glGenBuffers(1, &id);
		//將Buffer綁到指定VBO
		glBindBuffer(GL_ARRAY_BUFFER, id);
		//把數據複製到Buffer內 相當於複製到目前綁定的VBO
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
		//告訴Shader資料儲存的方式是甚麼
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