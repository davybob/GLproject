#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <array>

class Shader
{
public:
    unsigned int ID{ 0 };
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::string geometryCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        std::ifstream gShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            gShaderFile.open(geometryPath);
            std::stringstream vShaderStream, fShaderStream, gShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            gShaderStream << gShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            gShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
            geometryCode = gShaderStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        const char* gShaderCode = geometryCode.c_str();
        // 2. compile shaders
        unsigned int vertex{ 0 }, fragment{ 0 }, geometry{ 0 };
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
        //geometry = glCreateShader(GL_GEOMETRY_SHADER);
        //glShaderSource(geometry, 1, &gShaderCode, NULL);
        //glCompileShader(geometry);
        checkCompileErrors(geometry, "GEOMETRY");

        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glAttachShader(ID, geometry);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        //glDeleteShader(geometry);
    }
    
    // activate the shader
    // ------------------------------------------------------------------------
    void use()
    {
        glUseProgram(ID);
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
    void setFloat(const std::string& name, GLfloat value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    void setBoolNd(const std::string& name, int size, GLint *value) const
    {
        if (size == 1)
            glUniform1i(glGetUniformLocation(ID, name.c_str()), *value);
        else if (size == 2)
            glUniform2iv(glGetUniformLocation(ID, name.c_str()),1, value);
        else if (size == 3)
            glUniform3iv(glGetUniformLocation(ID, name.c_str()), 1, value);
        else if (size == 4)
            glUniform4iv(glGetUniformLocation(ID, name.c_str()), 1, value);

    }
    // ------------------------------------------------------------------------
    void setIntNd(const std::string& name, int size, GLint *value, int instances=1) const
    {
        if (size == 1)
            glUniform1i(glGetUniformLocation(ID, name.c_str()), *value);
        else if (size == 2)
            glUniform2iv(glGetUniformLocation(ID, name.c_str()), instances, value);
        else if (size == 3)
            glUniform3iv(glGetUniformLocation(ID, name.c_str()), instances, value);
        else if (size == 4)
            glUniform4iv(glGetUniformLocation(ID, name.c_str()), instances, value);
    }
    // ------------------------------------------------------------------------
    void setFloatNd(const std::string& name,int size, GLfloat *value, int instances = 1) const
    {
        if (size == 1)
            glUniform1fv(glGetUniformLocation(ID, name.c_str()),1, value);
        else if (size == 2)
            glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, value);
        else if (size == 3)
            glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, value);
        else if (size == 4)
            glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, value);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string& name, glm::mat4 value) {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }

    ~Shader() {
        glDeleteProgram(this->ID);
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
        else if (type == "GEOMETRY")
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else {
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