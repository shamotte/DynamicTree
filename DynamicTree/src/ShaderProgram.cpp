#include "ShaderProgram.h"
#include <sstream>
#include <fstream>
#include  <string>
#include <iostream>


std::string path  = "..\\POGame\\Shader\\";

static unsigned int CreateShader(unsigned int type, const char* source)
{
    unsigned int id = glCreateShader(type);
    GLCALL(glShaderSource(id, 1, &source, nullptr));

    GLCALL(glCompileShader(id););


    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int lenght;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lenght);
        char* message = (char*)alloca(lenght * sizeof(char));
        glGetShaderInfoLog(id, lenght, &lenght, message);
        std::cout << "FAILED TO COMPILE " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "SHADER\n";
        std::cout << message << "\n";
        glDeleteShader(id);
        return 0;
    }

    return id;
}


Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) : RendererID{glCreateProgram()}
{
    {
        std::ifstream file(path + vertexPath, std::ios::in);
        if (!file.good())
        {
            perror("cannot open vertex shader file");
        }
        std::stringstream source;
        source << file.rdbuf();

        file.close();
        GLCALL(unsigned int vs = CreateShader(GL_VERTEX_SHADER, source.str().c_str()););
        GLCALL(glAttachShader(RendererID, vs););
    }
    {
        std::ifstream file(path + fragmentPath, std::ios::in);
        if (!file.good())
        {
            perror("cannot open fragment shader file");
        }
        std::stringstream source;
        source << file.rdbuf();

        GLCALL(unsigned int fs = CreateShader(GL_FRAGMENT_SHADER, source.str().c_str()););
        file.close();
        GLCALL(glAttachShader(RendererID, fs););
    }
        GLCALL(glLinkProgram(RendererID););

}

Shader::~Shader()
{
    std::cout << "deleting shader";
    glDeleteProgram(RendererID);
}

void Shader::AddFragment(const std::string&)
{
}

void Shader::AddVertex(const std::string&)
{
}

void Shader::Bind() const
{
    GLCALL(glUseProgram(RendererID));
}

void Shader::Unbind() const
{
    GLCALL(glUseProgram(0));
}

void Shader::SetUniform4f(const std::string&name, float r, float g, float b, float a)
{
    GLint location = GetUniformLocation(name);
    Bind();
    GLCALL(glUniform4f(location, r, g, b, a));
    
   
}

void Shader::SetUniform1i(const std::string& name, int i)
{
    Bind(); 
    GLCALL(glUniform1i(GetUniformLocation(name), i));
}

void Shader::SetUniformMat4f(const std::string& name,const glm::mat4& mat)
{
    Bind();
    glUniformMatrix4fv(GetUniformLocation(name), 1, false, &mat[0][0]);
}

void Shader::SetUniform2f(const std::string& name, float f1, float f2)
{
    Bind();
    glUniform4f(GetUniformLocation(name), f1, f2,0,0);
}

GLint Shader::GetUniformLocation(const std::string& name)
{

    // TODO - cachowac jakos te lokalizaje by zaoszczedzic czas na wyszukiwaniu
    GLCALL(GLint location = glGetUniformLocation(RendererID, name.c_str()));
    if (location == -1) std::cout << "uniform " << name << " dosent exist\n";
    return location;
}

