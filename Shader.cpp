#include <Windows.h>
#include <fstream>
#include <sstream>
#include <map>
#include "GL/glew.h"
#include "Shader.h"

unsigned int Shader::compileShader(const char* source, GLenum type)
{
	// Wczytujemy z pliku
	std::ifstream shaderFile(source);
	std::stringstream shaderStream;
	shaderStream << shaderFile.rdbuf();
	std::string shaderSource = shaderStream.str();
	const char* shaderCode = shaderSource.c_str();

	// Tworzymy shader
	unsigned int id = glCreateShader(type);
	glShaderSource(id, 1, &shaderCode, nullptr);
	glCompileShader(id);
	
	// Sprawdzamy czy nie ma błędów
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* msg = new char[length];

		glGetShaderInfoLog(id, length, &length, msg);
		OutputDebugStringA("\n\n");
		if(type == GL_VERTEX_SHADER)
			OutputDebugStringA("Error compiling vertex shader:\n");
		else
			OutputDebugStringA("Error compiling fragment shader:\n");

		OutputDebugStringA(msg);
		OutputDebugStringA("\n\n");


		delete[] msg;
	}

	return id;
}

Shader::Shader(const char* vertexShader, const char* fragmentShader) 
	: vs_path(vertexShader), fs_path(fragmentShader)
{
	fs = 0;
	vs = 0;
	programID = 0;
}

void Shader::Init()
{
	programID = glCreateProgram();

	vs = compileShader(vs_path, GL_VERTEX_SHADER);
	fs = compileShader(fs_path, GL_FRAGMENT_SHADER);

	glAttachShader(programID, vs);
	glAttachShader(programID, fs);
	glLinkProgram(programID);
	glValidateProgram(programID);

	glDeleteShader(vs);
	glDeleteShader(fs);
}

void Shader::AddUniformLocation(const char* name)
{
	GLuint location = glGetUniformLocation(this->programID, name);
	this->uniforms[name] = location;
}

unsigned int Shader::GetProgramID()
{
	return programID;
}
