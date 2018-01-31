
#pragma once
#include "Core.h"

class Shader
{
public:
	GLuint Program;
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath = nullptr);
	void Use()
	{
		glUseProgram(this->Program);
	}

};
////////////////////////////////////////////////////////////////////////////////
