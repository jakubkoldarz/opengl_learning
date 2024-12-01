#include "Walec.h"
#include <math.h>

Walec::Walec(GLfloat x, GLfloat y, GLfloat z, GLfloat r, GLfloat h,Axis orientacja) : Obiekt(x, y, z) 
{
    // Domyślna prezycja rysowania wynosi 32
    _precyzja = 32;

    // Promień 
    _r = r;

    // Wysokość
    _h = h;

    // Domyślny kolor to czerwony
    _kolor[0] = 1.0f;
    _kolor[1] = 0.0f;
    _kolor[2] = 0.0f;

    // Domyślna orientacja
    _orientacja = orientacja;

    // Ustawienie wyświetlania podstaw
    _czyRysowacPodstawy = true;
}
void Walec::Render(bool czyWypelnic) const 
{
    glPolygonMode(GL_FRONT_AND_BACK, czyWypelnic ? GL_FILL : GL_LINE);
    glColor3fv(_kolor);

    if(_czyRysowacPodstawy)
        this->RysujPodstawy();

    this->RysujScianeBoczna();
}
void Walec::UstawPrecyzje(GLfloat precyzja)
{
    _precyzja = precyzja;
}
void Walec::UstawKolor(GLfloat r, GLfloat g, GLfloat b)
{
    _kolor[0] = r;
    _kolor[1] = g;
    _kolor[2] = b;
}
void Walec::UstawKolor(const GLfloat* v)
{
    _kolor[0] = v[0];
    _kolor[1] = v[1];
    _kolor[2] = v[2];
}
void Walec::UstawRysowaniePodstaw(bool czyRysowac)
{
    _czyRysowacPodstawy = czyRysowac;
}
void Walec::RysujPodstawy() const
{
    GLfloat angleStep = 2.0f * 3.14159265358979323846f / _precyzja;

    // Dolna podstawa walca
    glBegin(GL_TRIANGLE_FAN);
    {
        // Środek dolnej podstawy
        glVertex3f(_x, _y, _z);

        // Rysowanie punktów na obwodzie dolnej podstawy
        for (int i = 0; i <= _precyzja; ++i) 
        {
            switch (_orientacja)
            {
            case X:
            {
                GLfloat angle = i * angleStep;
                GLfloat dz = _z + sinf(angle) * _r;
                GLfloat dy = _y + cosf(angle) * _r;

                // Punkty na obwodzie dolnej podstawy
                glVertex3f(_x, dy, dz);
            }
            break;
            case Y:
            {
                GLfloat angle = i * angleStep;
                GLfloat dx = _x + cosf(angle) * _r;
                GLfloat dz = _z + sinf(angle) * _r;

                // Punkty na obwodzie dolnej podstawy
                glVertex3f(dx, _y, dz);
            }
            break;
            case Z:
            {
                GLfloat angle = i * angleStep;
                GLfloat dx = _x + cosf(angle) * _r;
                GLfloat dy = _y + sinf(angle) * _r;

                // Punkty na obwodzie dolnej podstawy
                glVertex3f(dx, dy, _z);
            }
            break;
            default: break;
            }
        }
    }
    glEnd();

    // Górna podstawa walca
    glBegin(GL_TRIANGLE_FAN);
    {
        // Środek górnej podstawy
        switch (_orientacja)
        {
        case X: glVertex3f(_x + _h, _y, _z);
            break;
        case Y: glVertex3f(_x, _y + _h, _z);
            break;
        case Z: glVertex3f(_x, _y, _z + _h);
            break;
        default:
            break;
        }

        // Rysowanie punktów na obwodzie górnej podstawy
        for (int i = 0; i <= _precyzja; ++i) {
            switch (_orientacja)
            {
            case X:
            {
                GLfloat angle = i * angleStep;
                GLfloat dz = _z + sinf(angle) * _r;
                GLfloat dy = _y + cosf(angle) * _r;

                // Punkty na obwodzie dolnej podstawy
                glVertex3f(_x + _h, dy, dz);
            }
            break;
            case Y:
            {
                GLfloat angle = i * angleStep;
                GLfloat dx = _x + cosf(angle) * _r;
                GLfloat dz = _z + sinf(angle) * _r;

                // Punkty na obwodzie dolnej podstawy
                glVertex3f(dx, _y + _h, dz);
            }
            break;
            case Z:
            {
                GLfloat angle = i * angleStep;
                GLfloat dx = _x + cosf(angle) * _r;
                GLfloat dy = _y + sinf(angle) * _r;

                // Punkty na obwodzie dolnej podstawy
                glVertex3f(dx, dy, _z + _h);
            }
            break;
            default: break;
            }
        }
    }
    glEnd();
}
void Walec::RysujScianeBoczna() const
{
    GLfloat angleStep = 2.0f * 3.14159265358979323846f / _precyzja;

    // Ściana boczna walca
    glBegin(GL_TRIANGLE_STRIP);
    {
        for (int i = 0; i <= _precyzja; ++i) {
            switch (_orientacja)
            {
            case X:
            {
                GLfloat angle = i * angleStep;
                GLfloat dz = _z + cosf(angle) * _r;
                GLfloat dy = _y + sinf(angle) * _r;

                // Wierzchołek dolny
                glVertex3f(_x, dy, dz);

                // Wierzchołek górny
                glVertex3f(_x + _h, dy, dz);
            }
            break;
            case Y:
            {
                GLfloat angle = i * angleStep;
                GLfloat dx = _x + cosf(angle) * _r;
                GLfloat dz = _z + sinf(angle) * _r;

                // Wierzchołek dolny
                glVertex3f(dx, _y, dz);

                // Wierzchołek górny
                glVertex3f(dx, _y + _h, dz);
            }
            break;
            case Z:
            {
                GLfloat angle = i * angleStep;
                GLfloat dx = _x + cosf(angle) * _r;
                GLfloat dy = _y + sinf(angle) * _r;

                // Wierzchołek dolny
                glVertex3f(dx, dy, _z);

                // Wierzchołek górny
                glVertex3f(dx, dy, _z + _h);
            }
            break;
            default: break;
            }
        }
    }
    glEnd();
}