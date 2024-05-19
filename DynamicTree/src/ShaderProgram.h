#pragma once
#include <string>

#include "glm/glm.hpp"
#include "Utils.h"

class Shader {
	unsigned int RendererID;

public:

	
	Shader(const std::string& vertexPath, const std::string& fragmentPath);
	~Shader();
	void AddFragment(const std::string&);
	void AddVertex(const std::string&);
	void Bind() const;
	void Unbind() const;
	void SetUniform4f(const std::string& name, float r, float g, float b, float a);
	void SetUniform1i(const std::string& name, int i);
	void SetUniformMat4f(const std::string& name, const glm::mat4& mat);
	void SetUniform2f(const std::string& name, float f1, float f2);
	GLint GetUniformLocation(const std::string& name);

};