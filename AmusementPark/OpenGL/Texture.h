#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <stb/stb_image.h>
#include <vector>
#include <string>

#include "Shader.h"

class Texture2D
{
public:
	GLuint id;
	Texture2D(const char* image)
	{
		int widthImg, heightImg, numColCh;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);

		if (!bytes)
			std::cout << "ERROR: Texture2D " << stbi_failure_reason() << ": " << image << std::endl;

		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.4f);

		if (numColCh == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthImg, heightImg, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes);
		else if (numColCh == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
		else if(numColCh == 1)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, widthImg, heightImg, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, bytes);


		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(bytes);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Bind(int slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, this->id);
	}
	void Unbind(int slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	void Delete()
	{
		glDeleteTextures(1, &id);
	}
};

class Texture3D
{
public:
	GLuint id;
	Texture3D(std::vector<std::string> paths)
	{
		int widthImg, heightImg, numColCh;
		stbi_set_flip_vertically_on_load(false);
		unsigned char* bytes;


		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


		for (int i = 0; i < paths.size(); i++)
		{
			bytes = stbi_load(paths[i].c_str(), &widthImg, &heightImg, &numColCh, 0);
			
			if (!bytes)
				std::cout <<"ERROR: Texture3D " << stbi_failure_reason() << ": " << paths[i] << std::endl;

			if (numColCh == 3)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, widthImg, heightImg, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes);
			else if (numColCh == 4)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, widthImg, heightImg, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);

			stbi_image_free(bytes);
		}

		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	void Bind(int slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_CUBE_MAP, this->id);
	}
	void Unbind(int slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
	void Delete()
	{
		glDeleteTextures(1, &id);
	}
};


#endif // !TEXTURE_H



