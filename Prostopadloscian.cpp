#include "Prostopadloscian.h"
#include <math.h>

Prostopadloscian::Prostopadloscian(GLfloat x, GLfloat y, GLfloat z, GLfloat rozmiar_X, GLfloat rozmiar_Y, GLfloat rozmiar_Z) : Obiekt(x, y, z)
{
    // Domyślna prezycja rysowania wynosi 32
    _precyzja = 32;

    // Długość w osi X
    _rozmiar_X = rozmiar_X;

    // Długość w osi Y
    _rozmiar_Y = rozmiar_Y;

    // Długość w osi Z
    _rozmiar_Z = rozmiar_Z;

    // Domyślny kolor to czerwony
    _kolor[0] = 1.0f;
    _kolor[1] = 0.0f;
    _kolor[2] = 0.0f;
}
void Prostopadloscian::Render(bool czyWypelnic) const
{
    glPolygonMode(GL_FRONT_AND_BACK, czyWypelnic ? GL_FILL : GL_LINE);
    glColor3fv(_kolor);

    this->RysujSciany();
}
void Prostopadloscian::UstawPrecyzje(GLfloat precyzja)
{
    _precyzja = precyzja;
}
void Prostopadloscian::UstawKolor(GLfloat r, GLfloat g, GLfloat b)
{
    _kolor[0] = r;
    _kolor[1] = g;
    _kolor[2] = b;
}
void Prostopadloscian::UstawKolor(const GLfloat* v)
{
    _kolor[0] = v[0];
    _kolor[1] = v[1];
    _kolor[2] = v[2];
}
void Prostopadloscian::RysujSciany() const
{
    // Dolna ściana bryły
    glBegin(GL_TRIANGLE_STRIP);
    {
        for (GLfloat x = 0; x <= _rozmiar_X; x += 5.0f)
        {
            glVertex3f(_x + x, _y, _z);
            glVertex3f(_x + x, _y, _z + _rozmiar_Z);
        }

    }
    glEnd();

    // Górna ściana bryły
    glBegin(GL_TRIANGLE_STRIP);
    {
        for (GLfloat x = 0; x <= _rozmiar_X; x += 5.0f)
        {
            glVertex3f(_x + x, _y + _rozmiar_Y, _z);
            glVertex3f(_x + x, _y + _rozmiar_Y, _z + _rozmiar_Z);
        }
    }
    glEnd();

    // Boczna 1 X ściana bryły
    glBegin(GL_TRIANGLE_STRIP);
    {
        for (GLfloat x = 0; x <= _rozmiar_X; x += 5.0f)
        {
            glVertex3f(_x + x, _y, _z);
            glVertex3f(_x + x, _y + _rozmiar_Y, _z);
        }
    }
    glEnd();

    // Boczna 2 X ściana bryły
    glBegin(GL_TRIANGLE_STRIP);
    {
        for (GLfloat x = 0; x <= _rozmiar_X; x += 5.0f)
        {
            glVertex3f(_x + x, _y, _z + _rozmiar_Z);
            glVertex3f(_x + x, _y + _rozmiar_Y, _z + _rozmiar_Z);
        }
    }
    glEnd();


    // Boczna 1 Z ściana bryły
    glBegin(GL_TRIANGLE_STRIP);
    {
        for (GLfloat z = 0; z <= _rozmiar_Z; z += 5.0f)
        {
            glVertex3f(_x, _y, _z + z);
            glVertex3f(_x, _y + _rozmiar_Y, _z + z);
        }
    }
    glEnd();

    // Boczna 2 Z ściana bryły
    glBegin(GL_TRIANGLE_STRIP);
    {
        for (GLfloat z = 0; z <= _rozmiar_Z; z += 5.0f)
        {
            glVertex3f(_x + _rozmiar_X, _y, _z + z);
            glVertex3f(_x + _rozmiar_X, _y + _rozmiar_Y, _z + z);
        }
    }
    glEnd();
        
}