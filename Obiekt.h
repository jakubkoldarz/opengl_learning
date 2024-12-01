#pragma once
#include "GL/glew.h"
#include "GL/GLU.h"
#include "GL/GL.h"

enum Axis { X, Y, Z };

class Obiekt
{
protected:
	GLfloat _x;
	GLfloat _y;
	GLfloat _z;
public:
	Obiekt(GLfloat x, GLfloat y, GLfloat z);
	virtual void Render(bool czyWypelnic = true) const = 0;
};

