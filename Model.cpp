#include <sstream>
#include <Windows.h>
#include <fstream>
#include "Model.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

        if (prefix == "v") 
        {  // Wierzchołek
            Vertex v;
            ss >> v.x >> v.y >> v.z;
            this->vertices.push_back(v);
        }
        else if (prefix == "vn") 
        {  // Normalna
            Vertex vn;
            ss >> vn.x >> vn.y >> vn.z;
            this->normals.push_back(vn);
        }
        else if (prefix == "vt") 
        {  // Współrzędne tekstur
            GLfloat u, v;
            ss >> u >> v;
            this->texCoords.push_back(u);
            this->texCoords.push_back(v);
        }
        else if (prefix == "f") 
        {  // Tworzenie ścian (trójkątów)
            GLuint idx[3], tex[3], norm[3];
            char slash;
            for (int i = 0; i < 3; i++) {
                ss >> idx[i] >> slash >> tex[i] >> slash >> norm[i];
                this->indices.push_back(idx[i] - 1);  // Indeksy w pliku .obj są 1-based
            }
        }
        else if (prefix == "mtllib")
        {
            std::string mtl_path;
            ss >> mtl_path;
            mtl_path = "obj\\" + mtl_path;
            this->material_path = new char[mtl_path.length()];
            strcpy_s(this->material_path, mtl_path.length() + 1, mtl_path.c_str());
        }
    }

    file.close();

    // Skalowanie wierzchołków w czasie ładowania modelu
    for (auto& vertex : this->vertices) {
        vertex.x *= scaleFactor;
        vertex.y *= scaleFactor;
        vertex.z *= scaleFactor;
    }

    return true;
}
bool Model::loadMTL() {
    OutputDebugStringA("\nMaterial Path:");
    OutputDebugStringA(this->material_path);
    std::ifstream file(this->material_path);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    Material* currentMaterial = nullptr;

    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "newmtl") {
            // Nowy materiał
            std::string materialName;
            ss >> materialName;
            currentMaterial = new Material();
            currentMaterial->name = materialName;
            this->materials.push_back(currentMaterial);
        }
        else if (prefix == "map_Kd") {  // Tekstura diffuse
            if (currentMaterial) {
                std::string textureFile;
                ss >> textureFile;
                textureFile = "obj\\" + textureFile;
                currentMaterial->textureFile = textureFile;
            }
        }
    }

    file.close();
    return true;
}
GLuint Model::loadTexture(const char* textureFile) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(textureFile, &width, &height, &nrChannels, 0);
    if (data) {
        OutputDebugStringA("\n\nTexture file loaded: ");
        OutputDebugStringA(textureFile);
        OutputDebugStringA("\n\n");

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        OutputDebugStringA("\n\nTexture loading error!\n\n\n");
    }
    stbi_image_free(data);

    return textureID;
}
Model::Model(const char* file, GLfloat scale) : path(file)
{
    // Współczynnik skali
    scaleFactor = scale;

    bool result = getModelFromFile();
    if (result) {
        // Ładowanie pliku MTL
        OutputDebugStringA(material_path);
        bool resultMTL = loadMTL();
        if(resultMTL)
            OutputDebugStringA("Material loaded successfully\n");
        else
            OutputDebugStringA("\nMaterial loading error\n");

        OutputDebugStringA("\nModel loaded successfully\n");
    }
    else {
        OutputDebugStringA("\nAn error occurred while loading the model from the file:");
        OutputDebugStringA(file);
        OutputDebugStringA("\n");
    }
}
Model::~Model()
{
}
void Model::Render()
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, this->vertices.data());
    glNormalPointer(GL_FLOAT, 0, this->normals.data());

    if (!this->texCoords.empty() && !materials.empty()) {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 0, this->texCoords.data());

        // Załaduj teksturę dla danego materiału
        Material* material = materials.front();  // Załóżmy, że materiał to pierwszy
        if (material && !material->textureFile.empty()) {
            GLuint texture = loadTexture(material->textureFile.c_str());
            if (texture != 0) {  // Sprawdź, czy tekstura została załadowana poprawnie
                glBindTexture(GL_TEXTURE_2D, texture);
            }
            else {
                OutputDebugStringA("Failed to load texture.\n");
            }
        }
        else {
            OutputDebugStringA("Material or texture path is invalid.\n");
        }
    }

    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, this->indices.data());

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    if (!this->texCoords.empty()) {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
}
