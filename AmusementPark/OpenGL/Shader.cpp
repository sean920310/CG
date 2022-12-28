#include "Shader.h"

Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
    std::string vertexCode = getFileContents(vertexFile);
    std::string fragmentCode = getFileContents(fragmentFile);

    const char* vertexSource = vertexCode.c_str();
    const char* fragmentSource = fragmentCode.c_str();

	//創建vertexShader程式
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//載入vertexShader程式
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	//編譯vertexShader程式
	glCompileShader(vertexShader);
	compileErrors(vertexShader, "VERTEX");

	//創建fragmentShader程式
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//載入fragmentShader程式
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	//編譯fragmentShader程式
	glCompileShader(fragmentShader);
	compileErrors(fragmentShader, "FRAGMENT");

	//創建最後所有shader的總和
	id = glCreateProgram();
	//將之前的shader連結上去
	glAttachShader(id, vertexShader);
	glAttachShader(id, fragmentShader);
	glLinkProgram(id);
	compileErrors(id, "PROGRAM");

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::Use()
{
	glUseProgram(id);
}

void Shader::Delete()
{
	glDeleteProgram(id);
}

void Shader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setMat4(const std::string& name, glm::mat4 value) const
{
	glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, false, &value[0][0]);
}

void Shader::setFloat3(const std::string& name, glm::vec3 value) const
{
	glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

void Shader::setDirLight(const DirLight* light, glm::mat4 viewMatrix)
{
	glm::vec4 pos = glm::inverse(viewMatrix) * light->direction;
	glUniform3fv(glGetUniformLocation(id, "dirLight.direction"), 1, glm::value_ptr(light->direction));
	glUniform3fv(glGetUniformLocation(id, "dirLight.ambient"), 1, &light->ambient[0]);
	glUniform3fv(glGetUniformLocation(id, "dirLight.diffuse"), 1, &light->diffuse[0]);
	glUniform3fv(glGetUniformLocation(id, "dirLight.specular"), 1, &light->specular[0]);
}

void Shader::setPosLight(const PosLight* light)
{
	glUniform3fv(glGetUniformLocation(id, "posLight.position"), 1, &light->position[0]);
	glUniform3fv(glGetUniformLocation(id, "posLight.ambient"), 1, &light->ambient[0]);
	glUniform3fv(glGetUniformLocation(id, "posLight.diffuse"), 1, &light->diffuse[0]);
	glUniform3fv(glGetUniformLocation(id, "posLight.specular"), 1, &light->specular[0]);

	glUniform1f(glGetUniformLocation(id, "posLight.constant"), light->constant);
	glUniform1f(glGetUniformLocation(id, "posLight.linear"), light->linear);
	glUniform1f(glGetUniformLocation(id, "posLight.quadratic"), light->quadratic);
}

void Shader::setSpotLight(const SpotLight* light)
{
	glUniform3fv(glGetUniformLocation(id, "spotLight.position"), 1, &light->position[0]);
	glUniform3fv(glGetUniformLocation(id, "posLight.direction"), 1, &light->direction[0]);
	glUniform3fv(glGetUniformLocation(id, "spotLight.ambient"), 1, &light->ambient[0]);
	glUniform3fv(glGetUniformLocation(id, "spotLight.diffuse"), 1, &light->diffuse[0]);
	glUniform3fv(glGetUniformLocation(id, "spotLight.specular"), 1, &light->specular[0]);

	glUniform1f(glGetUniformLocation(id, "spotLight.constant"), light->constant);
	glUniform1f(glGetUniformLocation(id, "spotLight.linear"), light->linear);
	glUniform1f(glGetUniformLocation(id, "spotLight.quadratic"), light->quadratic);

	glUniform1f(glGetUniformLocation(id, "spotLight.cutOff"), light->cutOff);
	glUniform1f(glGetUniformLocation(id, "spotLight.outerCutOff"), light->outerCutOff);
}

void Shader::compileErrors(unsigned int shader, const char* type)
{
	GLint hasCompiled;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_COMPILATION_ERROR for:" << type << "\n" << infoLog <<std::endl;
		}

	}
	else
	{
		glGetProgramiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_COMPILATION_ERROR for:" << type << "\n" << infoLog << std::endl;
		}
	}
}

std::string Shader::getFileContents(const char* fileName)
{
	std::ifstream in(fileName, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}