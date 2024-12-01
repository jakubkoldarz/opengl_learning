#pragma once
#include "Obiekt.h"

class Prostopadloscian : Obiekt
{
private:
	GLfloat _rozmiar_X;	// Oś X
	GLfloat _rozmiar_Y;	// Oś Y
	GLfloat _rozmiar_Z; // Oś Z

	GLfloat _precyzja; // Precyzja rysowania podstaw
	GLfloat _kolor[3]; // Kolor bryły

	void RysujSciany() const;
public:
	Prostopadloscian(GLfloat x, GLfloat y, GLfloat z, GLfloat glebia, GLfloat wysokosc, GLfloat szerokosc);
	void Render(bool czyWypelnic = true) const override;
	void UstawPrecyzje(GLfloat precyzja);
	void UstawKolor(GLfloat r, GLfloat g, GLfloat b);
	void UstawKolor(const GLfloat* v);
};

