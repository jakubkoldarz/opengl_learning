#include <sstream>
#include <Windows.h>
#include <fstream>
#include "Model.h"

bool Model::getModelFromFile()
{
    std::ifstream file(path);

    if (!file.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {  // Wierzchołek
            Vertex v;
            ss >> v.x >> v.y >> v.z;
            this->vertices.push_back(v);
        }
        else if (prefix == "vn") {  // Normalna
            Vertex vn;
            ss >> vn.x >> vn.y >> vn.z;
            this->normals.push_back(vn);
        }
        else if (prefix == "vt") {  // Współrzędne tekstur
            GLfloat u, v;
            ss >> u >> v;
            this->texCoords.push_back(u);
            this->texCoords.push_back(v);
        }
        else if (prefix == "f") {  // Tworzenie ścian (trójkątów)
            GLuint idx[3], tex[3], norm[3];
            char slash;
            for (int i = 0; i < 3; i++) {
                ss >> idx[i] >> slash >> tex[i] >> slash >> norm[i];
                this->indices.push_back(idx[i] - 1);  // Indeksy w pliku .obj są 1-based
            }
        }
    }

    file.close();

    //  Współczynnik skali
    float scaleFactor = 10.0f; 

    // Skalowanie wierzchołków w czasie ładowania modelu
    for (auto& vertex : this->vertices) {
        vertex.x *= scaleFactor;
        vertex.y *= scaleFactor;
        vertex.z *= scaleFactor;
    }

    return true;
}
Model::Model(const char* file) : path(file)
{
    bool result = getModelFromFile();

    if(result)
        OutputDebugStringA("\n\nModel loaded successfully\n\n");
    else
    {
        OutputDebugStringA("\n\nAn error occurred while loading the model from the file:");
        OutputDebugStringA(file);
        OutputDebugStringA("\n\n");
    }
}

void Model::Render()
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, this->vertices.data());
    glNormalPointer(GL_FLOAT, 0, this->normals.data());

    if (!this->texCoords.empty()) {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 0, this->texCoords.data());
    }

    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, this->indices.data());

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    if (!this->texCoords.empty()) {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
}
