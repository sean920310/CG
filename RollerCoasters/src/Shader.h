#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    unsigned int ID;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const char* vertexPath, const char* fragmentPath)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
    // activate the shader
    // ------------------------------------------------------------------------
    void use()
    {
        glUseProgram(ID);
    }

    void unUse()
    {
        glUseProgram(0);
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string& name, const glm::vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string& name, float x, float y, float z, float w) const
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }


    void setDirLight(bool enable)
    {
        GLfloat diffuse[4] = { 0.1f, 0.1f, 0.1f, 1.0f }, ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f }, lightPos[4];
        if (enable)
        {
            glGetLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
            glGetLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
        }

        glGetLightfv(GL_LIGHT0, GL_POSITION, lightPos);
        GLfloat view[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, view);
        glm::vec4 pos = glm::make_vec4(lightPos);
        glm::mat4 viewMatrix = glm::make_mat4(view);
        if (enable)
            pos = glm::inverse(viewMatrix) * pos;
        glUniform3fv(glGetUniformLocation(ID, "dirLight.position"), 1, glm::value_ptr(pos));
        glUniform4fv(glGetUniformLocation(ID, "dirLight.ambient"), 1, ambient);
        glUniform4fv(glGetUniformLocation(ID, "dirLight.diffuse"), 1, diffuse);
    }

    void setSpotLight(bool enable)
    {
        GLfloat diffuse[4] = { 0.0f, 0.0f, 0.0f, 1.0f }, ambient[4] = { 0.0f, 0.0f, 0.0f, 1.0f }, lightPos[4], lightDir[3];
        GLfloat constant, linear, quadratic,cutOff;
        if (enable)
        {
            glGetLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);
            glGetLightfv(GL_LIGHT2, GL_AMBIENT, ambient);
        }

        glGetLightfv(GL_LIGHT2, GL_POSITION, lightPos);
        glGetLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, lightDir);
        glGetLightfv(GL_LIGHT2, GL_CONSTANT_ATTENUATION, &constant);
        glGetLightfv(GL_LIGHT2, GL_LINEAR_ATTENUATION, &linear);
        glGetLightfv(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, &quadratic);
        glGetLightfv(GL_LIGHT2, GL_SPOT_CUTOFF, &cutOff);
        GLfloat view[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, view);
        glm::vec4 pos = glm::make_vec4(lightPos);
        glm::vec3 dir = glm::make_vec3(lightDir);
        glm::mat4 viewMatrix = glm::make_mat4(view);
        pos = glm::inverse(viewMatrix) * pos;
        dir = glm::inverse(viewMatrix) * glm::vec4(dir,0.0f);

        glUniform3fv(glGetUniformLocation(ID, "spotLight.position"), 1, glm::value_ptr(pos));
        glUniform3fv(glGetUniformLocation(ID, "spotLight.direction"), 1, glm::value_ptr(dir));
        glUniform4fv(glGetUniformLocation(ID, "spotLight.ambient"), 1, ambient);
        glUniform4fv(glGetUniformLocation(ID, "spotLight.diffuse"), 1, diffuse);
        glUniform1f(glGetUniformLocation(ID, "spotLight.constant"), constant);
        glUniform1f(glGetUniformLocation(ID, "spotLight.linear"), linear);
        glUniform1f(glGetUniformLocation(ID, "spotLight.quadratic"), quadratic);
        glUniform1f(glGetUniformLocation(ID, "spotLight.cutOff"), glm::cos(glm::radians(cutOff-5)));
        glUniform1f(glGetUniformLocation(ID, "spotLight.outerCutOff"), glm::cos(glm::radians(cutOff)));
    }
    void setHeadLight(bool enable)
    {
        GLfloat diffuse[4] = { 0.0f, 0.0f, 0.0f, 1.0f }, ambient[4] = { 0.0f, 0.0f, 0.0f, 1.0f }, lightPos[4], lightDir[3];
        GLfloat constant, linear, quadratic, cutOff;
        if (enable)
        {
            glGetLightfv(GL_LIGHT3, GL_DIFFUSE, diffuse);
            glGetLightfv(GL_LIGHT3, GL_AMBIENT, ambient);
        }

        glGetLightfv(GL_LIGHT3, GL_POSITION, lightPos);
        glGetLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, lightDir);
        glGetLightfv(GL_LIGHT3, GL_CONSTANT_ATTENUATION, &constant);
        glGetLightfv(GL_LIGHT3, GL_LINEAR_ATTENUATION, &linear);
        glGetLightfv(GL_LIGHT3, GL_QUADRATIC_ATTENUATION, &quadratic);
        glGetLightfv(GL_LIGHT3, GL_SPOT_CUTOFF, &cutOff);
        GLfloat view[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, view);
        glm::vec4 pos = glm::make_vec4(lightPos);
        glm::vec3 dir = glm::make_vec3(lightDir);
        glm::mat4 viewMatrix = glm::make_mat4(view);
        pos = glm::inverse(viewMatrix) * pos;
        dir = glm::inverse(viewMatrix) * glm::vec4(dir, 0.0f);

        glUniform3fv(glGetUniformLocation(ID, "headLight.position"), 1, glm::value_ptr(pos));
        glUniform3fv(glGetUniformLocation(ID, "headLight.direction"), 1, glm::value_ptr(dir));
        glUniform4fv(glGetUniformLocation(ID, "headLight.ambient"), 1, ambient);
        glUniform4fv(glGetUniformLocation(ID, "headLight.diffuse"), 1, diffuse);
        glUniform1f(glGetUniformLocation(ID, "headLight.constant"), constant);
        glUniform1f(glGetUniformLocation(ID, "headLight.linear"), linear);
        glUniform1f(glGetUniformLocation(ID, "headLight.quadratic"), quadratic);
        glUniform1f(glGetUniformLocation(ID, "headLight.cutOff"), glm::cos(glm::radians(cutOff - 5)));
        glUniform1f(glGetUniformLocation(ID, "headLight.outerCutOff"), glm::cos(glm::radians(cutOff)));
    }

     
private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};
#endif