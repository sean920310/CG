#include "WaterSurface.h"

WaterSurface::WaterSurface(Viewer* viewer)
{
	this->m_viewer = viewer;

	this->m_heightMaps = new std::vector<Texture2D*>(200);
	for (int i = 0; i < 200; i++)
	{
		std::string num = std::to_string(i);
		if (num.size() == 1) num = "00" + num;
		else if (num.size() == 2) num = "0" + num;
		std::string path = std::string("./Asset/Images/waves5/");
		path = path + num + ".png";
		m_heightMaps->at(i) = new Texture2D(path.c_str());
	}

	//* VAO
	//===================================================================================

	std::vector<GLuint>	element((VERTEX_COUNT - 1) * (VERTEX_COUNT - 1) * 2 * 3);
	std::vector<GLfloat> textureCoordinate((VERTEX_COUNT * VERTEX_COUNT) * 2);
	std::vector<GLfloat> vertices(VERTEX_COUNT * VERTEX_COUNT * 3);
	std::vector<GLfloat> normal(VERTEX_COUNT * VERTEX_COUNT * 3);

	for (int i = 0; i < VERTEX_COUNT; i++)
	{
		for (int j = 0; j < VERTEX_COUNT; j++)
		{
			vertices[(i * VERTEX_COUNT + j) * 3 + 0] = (float)j * SIZE / (VERTEX_COUNT - 1) - SIZE / 2;
			vertices[(i * VERTEX_COUNT + j) * 3 + 1] = 0.0f;
			vertices[(i * VERTEX_COUNT + j) * 3 + 2] = (float)i * SIZE / (VERTEX_COUNT - 1) - SIZE / 2;

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


	m_waterVAO = new VAO;
	m_waterVAO->Bind();

	VBO verticeVBO(&vertices[0], vertices.size() * sizeof(GLfloat));
	VBO normalVBO(&normal[0], normal.size() * sizeof(GLfloat));
	VBO textureCoordVBO(&textureCoordinate[0], textureCoordinate.size() * sizeof(GLfloat));

	EBO ebo(&element[0], element.size() * sizeof(GLuint));

	m_waterVAO->LinkAttrib(verticeVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	m_waterVAO->LinkAttrib(normalVBO, 1, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	m_waterVAO->LinkAttrib(textureCoordVBO, 2, 2, GL_FLOAT, 2 * sizeof(float), (void*)0);

	m_waterVAO->Unbind();
	verticeVBO.Unbind();
	normalVBO.Unbind();
	textureCoordVBO.Unbind();
	verticeVBO.Delete();
	normalVBO.Delete();
	textureCoordVBO.Delete();
	ebo.Unbind();


	//* FBO
	//===================================================================================

	this->reflectionFBO = new FBO;
	//gen framebuffer
	glGenFramebuffers(1, &this->reflectionFBO->fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, this->reflectionFBO->fbo);
	//gen texture
	glGenTextures(1, this->reflectionFBO->textures);
	glBindTexture(GL_TEXTURE_2D, this->reflectionFBO->textures[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_viewer->width, m_viewer->height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->reflectionFBO->textures[0], 0);
	//gen rbo
	glGenRenderbuffers(1, &this->reflectionFBO->rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, this->reflectionFBO->rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_viewer->width, m_viewer->height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->reflectionFBO->rbo);

	this->refractionFBO = new FBO;
	//gen framebuffer
	glGenFramebuffers(1, &this->refractionFBO->fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, this->refractionFBO->fbo);
	//gen texture
	glGenTextures(1, this->refractionFBO->textures);
	glBindTexture(GL_TEXTURE_2D, this->refractionFBO->textures[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_viewer->width, m_viewer->height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->refractionFBO->textures[0], 0);
	//gen rbo
	glGenRenderbuffers(1, &this->refractionFBO->rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, this->refractionFBO->rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_viewer->width, m_viewer->height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->refractionFBO->rbo);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void WaterSurface::Update()
{
	t += 0.5f;
	if (t >= 200) t = 0;
}

void WaterSurface::Draw(Shader* shader)
{
	shader->Use();
	this->m_heightMaps->at((int)t)->Bind(0);
	shader->setInt("heightMap", 0);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, this->reflectionFBO->textures[0]);
	shader->setInt("reflectTex", 4);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, this->refractionFBO->textures[0]);
	shader->setInt("refractTex", 5); 

	m_waterVAO->Bind();
	glDrawElements(GL_TRIANGLES, (VERTEX_COUNT - 1) * (VERTEX_COUNT - 1) * 2 * 3, GL_UNSIGNED_INT, 0);
}

void WaterSurface::ResizeFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->reflectionFBO->fbo);
	glBindTexture(GL_TEXTURE_2D, this->reflectionFBO->textures[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_viewer->width, m_viewer->height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindRenderbuffer(GL_RENDERBUFFER, this->reflectionFBO->rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_viewer->width, m_viewer->height);

	glBindFramebuffer(GL_FRAMEBUFFER, this->refractionFBO->fbo);
	glBindTexture(GL_TEXTURE_2D, this->refractionFBO->textures[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_viewer->width, m_viewer->height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindRenderbuffer(GL_RENDERBUFFER, this->refractionFBO->rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_viewer->width, m_viewer->height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
