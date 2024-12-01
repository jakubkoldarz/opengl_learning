#include <GL/glew.h>
#include <GL/GLU.h>
#include <GL/GL.h>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "Obiekt.h"
#include "Walec.h"
#include "Prostopadloscian.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <map>

#define M_PI        3.14159265358979323846
#define M_PI_2      1.57079632679489661923

struct Vertex {
    GLfloat x, y, z;
};

struct Model {
    std::vector<Vertex> vertices;
    std::vector<Vertex> normals;
    std::vector<GLfloat> texCoords;  // Współrzędne tekstur
    std::vector<GLuint> indices;     // Indeksy wierzchołków (trójkąty)
};

// Obsługa obracania obiektem
static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;

static GLfloat LightPosY = 2500.0f;
static GLfloat LightPosX = 4.0f;
static GLfloat LightPosZ = 4.0f;

// Obsługa rozmaru okna
static GLsizei lastHeight;
static GLsizei lastWidth;

// Obsługa kamery
static GLfloat xCam;
static GLfloat yCam;
static GLfloat zCam;

static Model world;
static Model cube;
//char path_world[] = "C:\\Users\\jakub\\Documents\\Studia\\Semestr 3\\Grafika Komputerowa\\Projekt\\obj\\surface.obj";
char path_world[]   = "C:\\Users\\jakub\\Documents\\Studia\\Semestr 3\\Grafika Komputerowa\\Projekt\\obj\\surface_2.obj";
char path_cube[]    = "C:\\Users\\jakub\\Documents\\Studia\\Semestr 3\\Grafika Komputerowa\\Projekt\\obj\\cube.obj";

static GLfloat cameraAngleX = 86.4f;     // Kąt w płaszczyźnie poziomej
static GLfloat cameraAngleY = -49.5f;     // Kąt w pionie (wysokość)
static GLfloat cameraRadius = 2500.0f;    // Promień (odległość od środka obiektu)

// Definicje kolorów obiektów
static GLfloat kolor_opon[3] = { 0.35f, 0.35f, 0.35f };
static GLfloat kolor_osi[3] = { 0.49f, 0.49f, 0.49f };
static GLfloat kolor_kabiny[3] = { 0.82f, 0.32f, 0.31f };
static GLfloat kolor_lampy[3] = { 0.31f, 0.82f, 0.27f };

// Definicje parametrów obiektów
static GLfloat rozmiar_opony = 10.0f;
static GLfloat grubosc_opony = 10.0f;
static GLfloat dlugosc_oski = 30.0f;
static GLfloat grubosc_oski = 5.0f;


static GLfloat macVal = 0.5f;
static GLfloat mscVal = 0.5f;

GLuint MatrixID;
GLuint ViewMatrixID;
GLuint ModelMatrixID;
GLuint LightID;
GLuint LightColorID;
GLuint MACID;
GLuint MSCID;

// Funkcja ładowania shaderów
GLuint LoadShader(const char* shaderPath, GLenum shaderType) {
    std::ifstream shaderFile(shaderPath);
    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    std::string shaderSource = shaderStream.str();
    const char* shaderCode = shaderSource.c_str();

    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderCode, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed: " << infoLog << std::endl;
    }

    return shader;
}
// Funkcja do ładowania programu shaderów
GLuint CreateShaderProgram(const char* vertexPath, const char* fragmentPath) {
    GLuint fragmentShader = LoadShader(fragmentPath, GL_FRAGMENT_SHADER);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed: " << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}
GLuint shaderProgram;
void SetupShaders() {
    shaderProgram = CreateShaderProgram("vertex_shader.glsl", "fragment_shader.glsl");
    glUseProgram(shaderProgram);
}
void SetupPerspective(GLsizei width, GLsizei height) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Pole widzenia w stopniach
    GLfloat fov = 45.0f;       

    // Proporcje okna
    GLfloat aspectRatio = (GLfloat)width / (GLfloat)height;  

    // Minimalna odległość widoczności
    GLfloat nearPlane = 0.1f;  

    // Maksymalna odległość widoczności
    GLfloat farPlane = 1000.0f;  

    gluPerspective(fov, aspectRatio, nearPlane, farPlane);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
void NormalizeAngle(GLfloat& angle) {
    while (angle < 0) angle += 2.0f * M_PI;
    while (angle > 2.0f * M_PI) angle -= 2.0f * M_PI;
}
void RysujOs(GLfloat x, GLfloat y, GLfloat z)
{
    Walec k1(x, y, -dlugosc_oski + z, rozmiar_opony, grubosc_opony, Axis::Z);
    k1.UstawKolor(kolor_opon);
    k1.Render();

    Walec k2(x, y, dlugosc_oski + z, rozmiar_opony, grubosc_opony, Axis::Z);
    k2.UstawKolor(kolor_opon);
    k2.Render();

    Walec os1(x, y, -dlugosc_oski + grubosc_opony + z, grubosc_oski, dlugosc_oski * 2 - grubosc_opony, Axis::Z);
    os1.UstawKolor(kolor_osi);
    os1.UstawRysowaniePodstaw(false);
    os1.Render();
}
void RysujLampy(GLfloat rozmiar_lampy, GLfloat grubosc_lampy)
{
    Walec lampa1(45.0f, 35.0f / 2.0f, -15.0f, rozmiar_lampy, grubosc_lampy);
    lampa1.UstawKolor(kolor_lampy);
    lampa1.Render();

    Walec lampa2(45.0f, 35.0f / 2.0f, 25.0f, rozmiar_lampy, grubosc_lampy);
    lampa2.UstawKolor(kolor_lampy);
    lampa2.Render();
}
void RysujAntene()
{
    Walec antena1(-60.0f, 50.0f, 15.0f, 2.0f, 25.0f, Axis::Y);
    antena1.UstawKolor(kolor_opon);
    antena1.Render();

    Walec antena2(-60.0f, 75.0f, 15.0f, 1.0f, 25.0f, Axis::Y);
    antena2.UstawKolor(kolor_opon);
    antena2.Render();
}
void RysujObudowe()
{
    Prostopadloscian p1(-75.0f, 0.0f, -20.0f, 55.0f, 35.0f, dlugosc_oski * 2 - grubosc_opony);
    p1.UstawKolor(kolor_kabiny);
    p1.Render();

    Prostopadloscian p2(-20.0f, 5.0f, -20.0f + grubosc_opony / 2.0f, 25.0f, 25.0f, dlugosc_oski * 2 - 2 * grubosc_opony);
    p2.UstawKolor(kolor_opon);
    p2.Render();

    Prostopadloscian p3(5.0f, 0.0f, -20.0f, 40.0f, 35.0f, dlugosc_oski * 2 - grubosc_opony);
    p3.UstawKolor(kolor_kabiny);
    p3.Render();

    Prostopadloscian p4(-75.0f, 35.0f, -20.0f, 40.0f, 15.0f, dlugosc_oski * 2 - grubosc_opony);
    p4.UstawKolor(kolor_kabiny);
    p4.Render();
}
void RysujRury()
{
    glPushMatrix();
    glRotatef(45.0f, 1.0f, 0.0f, 0.0f);

    Walec k1(-15.0f, 6.0f, -45.0f, 3.0f, 20.0f, Axis::Z);
    k1.UstawKolor(kolor_osi);
    k1.Render();

    Walec k2(0.0f, 6.0f, -45.0f, 3.0f, 20.0f, Axis::Z);
    k2.UstawKolor(kolor_osi);
    k2.Render();

    Walec k5(-15.0f, -3.0, -43.00, 3.0f, 30.0f, Axis::Z);
    k5.UstawKolor(kolor_osi);
    k5.Render();

    Walec k6(0.0f, -3.0, -43.00, 3.0f, 30.0f, Axis::Z);
    k6.UstawKolor(kolor_osi);
    k6.Render();

    glPopMatrix();

    glPushMatrix();
    glRotatef(-45.0f, 1.0f, 0.0f, 0.0f);

    Walec k3(-15.0f, -1.0f, 32.0f, 3.0f, 20.0f, Axis::Z);
    k3.UstawKolor(kolor_osi);
    k3.Render();

    Walec k4(0.0f, -1.0f, 32.0f, 3.0f, 20.0f, Axis::Z);
    k4.UstawKolor(kolor_osi);
    k4.Render();

    Walec k7(-15.0f, -10.00, 20.00, 3.0f, 30.0f, Axis::Z);
    k7.UstawKolor(kolor_osi);
    k7.Render();

    Walec k8(0.0f, -10.00, 20.00, 3.0f, 30.0f, Axis::Z);
    k8.UstawKolor(kolor_osi);
    k8.Render();

    glPopMatrix();
}
void RysujObiekty() 
{
    GLfloat dy = -7.0f;
    RysujOs(-60.0f, dy, 0);
    RysujOs(-35.0f, dy, 0);
    RysujOs(25.0f, dy, 0);

    RysujLampy(8.0f, 3.0f);
    RysujAntene();
    RysujObudowe();
    RysujRury();
}
void ChangeSize(GLsizei w, GLsizei h)
{
    GLfloat nRange = 100.0f;
    GLfloat fAspect;
    // Prevent a divide by zero
    if (h == 0)
        h = 1;

    lastWidth = w;
    lastHeight = h;

    fAspect = (GLfloat)w / (GLfloat)h;

    // Set Viewport to window dimensions
    glViewport(0, 0, w, h);
    SetupPerspective(w, h);

    // Reset coordinate system
    /*glMatrixMode(GL_PROJECTION);
    glLoadIdentity();*/

    // Establish clipping volume (left, right, bottom, top, near, far)
    /*if (w <= h)
        glOrtho(-nRange, nRange, -nRange * h / w, nRange * h / w, -nRange, nRange);
    else
        glOrtho(-nRange * w / h, nRange * w / h, -nRange, nRange, -nRange, nRange);*/

    // Establish perspective: 
    /*
    gluPerspective(60.0f,fAspect,1.0,400);
    */

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
bool LoadOBJ(const char* path, Model& model) {
    std::ifstream file(path);

    if (!file.is_open()) {
        OutputDebugStringA("Nie można załadować pliku\n");
        return false;
    }
    else
    {
        OutputDebugStringA("Wczytano plik\n");
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {  // Wierzchołek
            Vertex v;
            ss >> v.x >> v.y >> v.z;
            model.vertices.push_back(v);
        }
        else if (prefix == "vn") {  // Normalna
            Vertex vn;
            ss >> vn.x >> vn.y >> vn.z;
            model.normals.push_back(vn);
        }
        else if (prefix == "vt") {  // Współrzędne tekstur
            GLfloat u, v;
            ss >> u >> v;
            model.texCoords.push_back(u);
            model.texCoords.push_back(v);
        }
        else if (prefix == "f") {  // Tworzenie ścian (trójkątów)
            GLuint idx[3], tex[3], norm[3];
            char slash;
            for (int i = 0; i < 3; i++) {
                ss >> idx[i] >> slash >> tex[i] >> slash >> norm[i];
                model.indices.push_back(idx[i] - 1);  // Indeksy w pliku .obj są 1-based
            }
        }
    }

    file.close();

    float scaleFactor = 10.0f; // Przykładowy współczynnik skali

    // Skalowanie wierzchołków w czasie ładowania modelu
    for (auto& vertex : model.vertices) {
        vertex.x *= scaleFactor;
        vertex.y *= scaleFactor;
        vertex.z *= scaleFactor;
    }

    return true;
}
void RenderModel(const Model& model) {
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, model.vertices.data());
    glNormalPointer(GL_FLOAT, 0, model.normals.data());

    if (!model.texCoords.empty()) {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 0, model.texCoords.data());
    }

    try
    {
        glDrawElements(GL_TRIANGLES, model.indices.size(), GL_UNSIGNED_INT, model.indices.data());
    }
    catch (...)
    {

    }

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    if (!model.texCoords.empty()) {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
}
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_KEYDOWN:
        switch (wParam)
        {
            // Sterowanie kamerą
        case 'A': cameraAngleX -= 0.05f; break;
        case 'D': cameraAngleX += 0.05f; break;
        case 'W': cameraAngleY += 0.05f; break;
        case 'S': cameraAngleY -= 0.05f; break;
        case 'Q': cameraRadius -= 50.0f;  break;
        case 'E': cameraRadius += 50.0f; break;

            // Sterowanie światłem
        case VK_UP:     LightPosZ += 10.0f; break;
        case VK_DOWN:   LightPosZ -= 10.0f; break;
        case VK_LEFT:   LightPosX += 10.0f; break;
        case VK_RIGHT:  LightPosX -= 10.0f; break;
        case 'O':       LightPosY += 50.0f; break;
        case 'P':       LightPosY -= 50.0f; break;
        case 'N':       mscVal = mscVal >= 1.0f ? 1.0f : mscVal + 0.1f; break;
        case 'M':       mscVal = mscVal <= 0.0f ? 0.0f : mscVal - 0.1f; break;
        case 'K':       macVal = macVal >= 1.0f ? 1.0f : macVal + 0.1f; break;
        case 'L':       macVal = macVal <= 0.0f ? 0.0f : macVal - 0.1f; break;
        }

        NormalizeAngle(cameraAngleX);
        NormalizeAngle(cameraAngleY);

        xRot = (int)xRot % 360;
        yRot = (int)yRot % 360;

        InvalidateRect(hwnd, NULL, FALSE);
        break;
    case WM_SIZE:
        ChangeSize(LOWORD(lParam), HIWORD(lParam));
        break;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
//void Render(HWND hwnd) 
//{
//    // Wyczyść bufor koloru i głębi
//    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glLoadIdentity();
//
//    // Obliczenie pozycji kamery na podstawie kątów
//    xCam = cameraRadius * cosf(cameraAngleY) * sinf(cameraAngleX);
//    yCam = cameraRadius * sinf(cameraAngleY);
//    zCam = cameraRadius * cosf(cameraAngleY) * cosf(cameraAngleX);
//
//    GLfloat upY = (cosf(cameraAngleY) >= 0.0f) ? 1.0f : -1.0f;
//
//    gluLookAt(
//        xCam, yCam, zCam,
//        1.0f, 1.0f, 1.0f, // Punkt patrzenia
//        0.0f, upY, 0.0f  // Wektor "góry"
//    );
//    
//    // Przesunięcie w dół obiektu
//    glTranslatef(0.0f, -30.0f, 0.0f);
//
//    glPushMatrix();
//
//    // Rysowanie obiektów
//    //RysujObiekty();
//
//    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
//    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
//    GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
//
//
//    RenderModel(model);
//
//    glPopMatrix();
//    glFlush();
//
//    // Zamiana buforów
//    HDC hdc = GetDC(hwnd);
//    SwapBuffers(hdc);
//    ReleaseDC(hwnd, hdc);
//}
void Render(HWND hwnd) {
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wyświetlanie obrysu

    // Obliczanie pozycji kamery na podstawie kątów
    xCam = cameraRadius * cosf(cameraAngleY) * sinf(cameraAngleX);
    yCam = cameraRadius * sinf(cameraAngleY);
    zCam = cameraRadius * cosf(cameraAngleY) * cosf(cameraAngleX);

    GLfloat upY = (cosf(cameraAngleY) >= 0.0f) ? 1.0f : -1.0f;

    gluLookAt(
        xCam, yCam, zCam,   // Pozycje kamery
        1.0f, 1.0f, 1.0f,   // Punkt patrzenia
        0.0f, upY, 0.0f     // Wektor "góry"
    );

    // Wyszukiwanie uniformów w programie shaderów
    glm::mat4 ProjectionMatrix = glm::perspective(45.0f, 1200.0f / 700.0f, 0.1f, 100000.0f);
    glm::mat4 ViewMatrix = glm::lookAt(glm::vec3(xCam, yCam, zCam), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, upY, 0.0f));
    glm::mat4 ModelMatrix = glm::mat4(1.0);
    glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
    glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

    glm::vec3 lightPos = glm::vec3(LightPosX, LightPosY, LightPosZ);
    glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

    glUniform3f(MSCID, mscVal, mscVal, mscVal);
    glUniform3f(MACID, macVal, macVal, macVal);

    // Rysowanie obiektu
    RenderModel(world);

    glFlush();

    // Zamiana buforów
    HDC hdc = GetDC(hwnd);
    SwapBuffers(hdc);
    ReleaseDC(hwnd, hdc);

}
void SetupOpenGL(HWND hwnd) 
{
    HDC hdc = GetDC(hwnd);
    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 32; // Obsługa bufora głębi

    int pixelFormat = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pixelFormat, &pfd);

    HGLRC hrc = wglCreateContext(hdc);
    wglMakeCurrent(hdc, hrc);

    // Inicjalizacja GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
        return;
    }

    // Włącz bufor głębi
    glEnable(GL_DEPTH_TEST);

    // Ustawienie macierzy projekcji perspektywicznej
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1200.0 / 700.0, 0.1, 100.0);  // FOV, aspect ratio, near, far
    glMatrixMode(GL_MODELVIEW);  // Przejdź do macierzy widoku
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) 
{
    const WCHAR* CLASS_NAME = L"OpenGLWindowClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"Projekt na 5", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1200, 700, nullptr, nullptr, hInstance, nullptr);

    ShowWindow(hwnd, nCmdShow);
    SetupOpenGL(hwnd);

    ChangeSize(1200, 700);
    
    // Wczytanie z pliku
    LoadOBJ(path_world, world);
    LoadOBJ(path_cube, cube);

    // Shadery
    shaderProgram = CreateShaderProgram("vertex_shader.glsl", "fragment_shader.glsl");

    MatrixID = glGetUniformLocation(shaderProgram, "MVP");
    ViewMatrixID = glGetUniformLocation(shaderProgram, "V");
    ModelMatrixID = glGetUniformLocation(shaderProgram, "M");
    LightID = glGetUniformLocation(shaderProgram, "LightPosition_worldspace");
    LightColorID = glGetUniformLocation(shaderProgram, "LightColor");
    MACID = glGetUniformLocation(shaderProgram, "MAC");
    MSCID = glGetUniformLocation(shaderProgram, "MSC");

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        glUseProgram(shaderProgram);
        Render(hwnd);
    }

    return 0;
}