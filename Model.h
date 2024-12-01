#pragma once

#include <vector>
#include "GL/glew.h"

struct Vertex 
{
    GLfloat x, y, z;
};

class Material 
{
public:
    std::string name;
    std::string textureFile;
};

class Model
{
	const char* path;
    char* material_path;
    GLfloat scaleFactor{};

    std::vector<Vertex> vertices;
    std::vector<Vertex> normals;
    std::vector<GLfloat> texCoords;  // Współrzędne tekstur
    std::vector<GLuint> indices;     // Indeksy wierzchołków (trójkąty)

    std::vector<Material*> materials;

    bool getModelFromFile();
    bool loadMTL();
    GLuint loadTexture(const char* textureFile);
public:
    Model(const char* file, GLfloat scale = 1.0f); 
    ~Model();
    void Render();
};

