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
#include "Shader.h"
#include "Model.h"
#include "Prostopadloscian.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <map>

#define M_PI        3.14159265358979323846
#define M_PI_2      1.57079632679489661923

// Obsługa obracania obiektem
static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;

static GLfloat carPosX = 0.0f;
static GLfloat carPosY = 0.0f;
static GLfloat carPosZ = 0.0f;

static GLfloat LightPosY = 7500.0f;
static GLfloat LightPosX = 4.0f;
static GLfloat LightPosZ = 4.0f;

// Obsługa rozmaru okna
static GLsizei lastHeight;
static GLsizei lastWidth;

// Obsługa kamery
static GLfloat xCam;
static GLfloat yCam;
static GLfloat zCam;

char path_world[]   = "C:\\Users\\jakub\\Documents\\Studia\\Semestr 3\\Grafika Komputerowa\\Projekt\\obj\\surface3.obj";
char path_car[]    = "C:\\Users\\jakub\\Documents\\Studia\\Semestr 3\\Grafika Komputerowa\\Projekt\\obj\\crate.obj";

static Model world(path_world, 2000);
static Model car(path_car, 50);

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

static Shader shader("vs.shader", "fs.shader");

void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}
void GLCheckError()
{
    while (GLenum error = glGetError())
    {
        char msg[50];
        sprintf_s(msg, "[OpenGL Error] %d", error);
        OutputDebugStringA(msg);
    }
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

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    float carSpeed = 50.0f;

    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_KEYDOWN:
        switch (wParam)
        {
            // Sterowanie pojazdem
        case 'S': carPosZ -= carSpeed; break; // Do przodu
        case 'W': carPosZ += carSpeed; break; // Do tyłu
        case 'D': carPosX -= carSpeed; break; // W lewo
        case 'A': carPosX += carSpeed; break; // W prawo
        case 'R': carPosY += carSpeed; break; // Do góry
        case 'F': carPosY -= carSpeed; break; // W dół

            // Sterowanie kamerą
        case VK_UP:     cameraAngleY += 0.05f; break;
        case VK_DOWN:   cameraAngleY -= 0.05f; break;
        case VK_LEFT:   cameraAngleX -= 0.05f; break;
        case VK_RIGHT:  cameraAngleX += 0.05f; break;

            // Sterowanie światłem
        case 'O':       LightPosY += 50.0f; break;
        case 'P':       LightPosY -= 50.0f; break;
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
void Render(HWND hwnd) {
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wyświetlanie obrysu

    // Obliczanie pozycji kamery na podstawie kątów
    xCam = cameraRadius * cosf(cameraAngleY) * sinf(cameraAngleX);
    yCam = cameraRadius * sinf(cameraAngleY);
    zCam = cameraRadius * cosf(cameraAngleY) * cosf(cameraAngleX);

    GLfloat upY = (cosf(cameraAngleY) >= 0.0f) ? 1.0f : -1.0f;

    // Pozycja łazika
    glm::vec3 roverPosition = glm::vec3(carPosX, carPosY, carPosZ);

    // Wektor, który określa kierunek patrzenia kamery (np. wsteczny kierunek łazika)
    //glm::vec3 cameraOffset = roverPosition * 50.0f;

    // Ustawienie pozycji kamery (przesunięcie kamery o 'cameraOffset' w stosunku do łazika)
    //glm::vec3 cameraPosition = roverPosition - cameraOffset;

    // Ustawienie kamery, aby patrzyła na łazik
    //glm::mat4 ViewMatrix = glm::lookAt(cameraPosition, roverPosition, glm::vec3(0, 1, 0));

    // Ustawienie macierzy projekcji
    glm::mat4 ProjectionMatrix = glm::perspective(45.0f, 1200.0f / 700.0f, 0.1f, 100000.0f);
    //glm::mat4 ViewMatrix = glm::lookAt(glm::vec3(xCam, yCam, zCam), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, upY, 0.0f));
    glm::mat4 ViewMatrix = glm::lookAt(glm::vec3(xCam, yCam, zCam), roverPosition, glm::vec3(0.0f, upY, 0.0f));

    // Rysowanie świata (bez przesunięcia)
    //glm::mat4 ModelMatrix_world = glm::mat4(1.0f); // brak przesunięcia dla świata
    glm::mat4 ModelMatrix_world = glm::translate(glm::mat4(1.0f), glm::vec3(0, -200.0f, 0));
    glm::mat4 MVP_world = ProjectionMatrix * ViewMatrix * ModelMatrix_world;

    // Wysyłanie macierzy projekcji i widoku do shadera
    glUniformMatrix4fv(shader.uniforms["MVP"], 1, GL_FALSE, &MVP_world[0][0]);
    glUniformMatrix4fv(shader.uniforms["M"], 1, GL_FALSE, &ModelMatrix_world[0][0]);
    glUniformMatrix4fv(shader.uniforms["V"], 1, GL_FALSE, &ViewMatrix[0][0]);
    
    // Ustawiamy pozycję światła
    glm::vec3 lightPos = glm::vec3(LightPosX, LightPosY, LightPosZ);
    glUniform3f(shader.uniforms["LightPosition_worldspace"], lightPos.x, lightPos.y, lightPos.z);

    // Ustawiamy kolor świata
    //MaterialDiffuseColor
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    glUniform3f(shader.uniforms["MaterialDiffuseColor"], color.x, color.y, color.z);

    // Rysowanie świata
    world.Render();

    // Poruszanie się łazikiem (pozycjonowanie pojazdu)
    glm::mat4 ModelMatrix_car = glm::translate(glm::mat4(1.0f), glm::vec3(carPosX, carPosY, carPosZ)); 
    glm::mat4 MVP_car = ProjectionMatrix * ViewMatrix * ModelMatrix_car;

    // Wysyłanie zaktualizowanej macierzy do shadera dla samochodu
    glUniformMatrix4fv(shader.uniforms["MVP"], 1, GL_FALSE, &MVP_car[0][0]);
    glUniformMatrix4fv(shader.uniforms["M"], 1, GL_FALSE, &ModelMatrix_car[0][0]);
    glUniformMatrix4fv(shader.uniforms["V"], 1, GL_FALSE, &ViewMatrix[0][0]);

    glUniform1i(shader.uniforms["myTextureSampler"], 0);
    // Ustawiamy kolor łazika
    color = glm::vec3(1.0f, 0.0f, 0.0f);
    glUniform3f(shader.uniforms["MaterialDiffuseColor"], color.x, color.y, color.z);

    // Rysowanie przesuniętego łazika
    car.Render();

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
    
    shader.Init();

    shader.AddUniformLocation("MVP");
    shader.AddUniformLocation("V");
    shader.AddUniformLocation("M");
    shader.AddUniformLocation("LightPosition_worldspace");
    shader.AddUniformLocation("MaterialDiffuseColor");
    shader.AddUniformLocation("myTextureSampler");

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        glUseProgram(shader.GetProgramID());
        
        Render(hwnd);
    }

    return 0;
}