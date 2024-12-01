#pragma once

#include <map>
#include <string>
#include "GL/GL.h"
#include "GL/GLU.h"
#include "GL/glew.h"

class Shader
{
	unsigned int programID;
	unsigned int vs;
	unsigned int fs;
	const char* vs_path;
	const char* fs_path;

	unsigned int compileShader(const char* source, GLenum type);
public:
	std::map<std::string, GLuint> uniforms;
	Shader(const char* vertexShader, const char* fragmentShader);
	void Init();
	void AddUniformLocation(const char* name);
	unsigned int GetProgramID();
};

