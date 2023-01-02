#ifndef SHADER_H
#define SHADER_H
	
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "Light.h"


class Shader
{
public:
	GLuint id;
	Shader(const char* vertexFile, const char* fragmentFile);

	void Use();
	void Delete();

	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setMat4(const std::string& name, glm::mat4 value) const;
	void setFloat3(const std::string& name, glm::vec3 value) const;

	
	void setDirLight(const DirLight* light);
	void setPosLight(const PosLight* light);
	void setSpotLight(const SpotLight* light);

private:
	void compileErrors(unsigned int shader, const char* type);
	std::string getFileContents(const char* fileName);
};


#endif // !SHADER_H


