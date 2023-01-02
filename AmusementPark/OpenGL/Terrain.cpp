#include "Terrain.h"

Terrain::Terrain(int gridX, int gridZ, const std::string& texturePath)
{
	m_texture = new Texture2D(texturePath.c_str());
	x = gridX * SIZE;
	z = gridZ * SIZE;


	std::vector<GLuint>	element((VERTEX_COUNT - 1) * (VERTEX_COUNT - 1) * 2 * 3);
	std::vector<GLfloat> textureCoordinate((VERTEX_COUNT * VERTEX_COUNT) * 2);
	std::vector<GLfloat> vertices(VERTEX_COUNT * VERTEX_COUNT * 3);
	std::vector<GLfloat> normal(VERTEX_COUNT * VERTEX_COUNT * 3);

	for (int i = 0; i < VERTEX_COUNT; i++)
	{
		for (int j = 0; j < VERTEX_COUNT; j++)
		{
			vertices[(i * VERTEX_COUNT + j) * 3 + 0] = (float)j * SIZE / (VERTEX_COUNT - 1) + x;
			vertices[(i * VERTEX_COUNT + j) * 3 + 1] = 0.0f;
			vertices[(i * VERTEX_COUNT + j) * 3 + 2] = (float)i * SIZE / (VERTEX_COUNT - 1) + z;

			normal[(i * VERTEX_COUNT + j) * 3 + 0] = 0.0f;
			normal[(i * VERTEX_COUNT + j) * 3 + 1] = 1.0f;
			normal[(i * VERTEX_COUNT + j) * 3 + 2] = 0.0f;

			textureCoordinate[(i * VERTEX_COUNT + j) * 2 + 0] = (1.0f / (VERTEX_COUNT - 1)) * i;
			textureCoordinate[(i * VERTEX_COUNT + j) * 2 + 1] = (1.0f / (VERTEX_COUNT - 1)) * j;

			if (i + 1 != VERTEX_COUNT && j + 1 != VERTEX_COUNT)
			{
				element[(i * (VERTEX_COUNT - 1) + j) * 3 + 0] = i * VERTEX_COUNT + j;
				element[(i * (VERTEX_COUNT - 1) + j) * 3 + 1] = i * VERTEX_COUNT + j + 1;
				element[(i * (VERTEX_COUNT - 1) + j) * 3 + 2] = (i + 1) * VERTEX_COUNT + j;

				element[((VERTEX_COUNT - 1) * (VERTEX_COUNT - 1) * 3) + (i * (VERTEX_COUNT - 1) + j) * 3 + 0] = (i + 1) * VERTEX_COUNT + j + 1;
				element[((VERTEX_COUNT - 1) * (VERTEX_COUNT - 1) * 3) + (i * (VERTEX_COUNT - 1) + j) * 3 + 1] = (i + 1) * VERTEX_COUNT + j;
				element[((VERTEX_COUNT - 1) * (VERTEX_COUNT - 1) * 3) + (i * (VERTEX_COUNT - 1) + j) * 3 + 2] = i * VERTEX_COUNT + j + 1;
			}
		}
	}


	m_vao = new VAO;
	m_vao->Bind();

	VBO verticeVBO(&vertices[0],vertices.size()*sizeof(GLfloat));
	VBO normalVBO(&normal[0], normal.size() * sizeof(GLfloat));
	VBO textureCoordVBO(&textureCoordinate[0], textureCoordinate.size() * sizeof(GLfloat));

	EBO ebo(&element[0], element.size() * sizeof(GLuint));

	m_vao->LinkAttrib(verticeVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	m_vao->LinkAttrib(normalVBO, 1, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	m_vao->LinkAttrib(textureCoordVBO, 2, 2, GL_FLOAT, 2 * sizeof(float), (void*)0);

	m_vao->Unbind();
	verticeVBO.Unbind();
	normalVBO.Unbind();
	textureCoordVBO.Unbind();
	verticeVBO.Delete();
	normalVBO.Delete();
	textureCoordVBO.Delete();
	ebo.Unbind();
}

Terrain::~Terrain()
{
	if (m_texture)
		delete m_texture;
	if (m_vao)
		delete m_vao;
}

void Terrain::Draw(Shader* shader)
{
	shader->Use();

	m_texture->Bind(0);
	shader->setInt("tex", 0);

	m_vao->Bind();
	glDrawElements(GL_TRIANGLES, (VERTEX_COUNT - 1) * (VERTEX_COUNT - 1) * 2 * 3, GL_UNSIGNED_INT, 0);
}
