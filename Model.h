#pragma once

#include <vector>
#include "GL/glew.h"

struct Vertex {
    GLfloat x, y, z;
};

class Model
{
	const char* path;
    std::vector<Vertex> vertices;
    std::vector<Vertex> normals;
    std::vector<GLfloat> texCoords;  // Współrzędne tekstur
    std::vector<GLuint> indices;     // Indeksy wierzchołków (trójkąty)

    bool getModelFromFile();
public:
    Model(const char* file); 
    void Render();
};

