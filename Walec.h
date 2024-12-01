#pragma once
#include "Obiekt.h"

class Walec : Obiekt
{
private:
	GLfloat _r; // Promień walca
	GLfloat _h; // Wysokość walca
	GLfloat _precyzja; // Precyzja rysowania podstaw
	GLfloat _kolor[3]; // Kolor bryły
	GLfloat _czyRysowacPodstawy;
	Axis _orientacja; // Orientacja figury

	void RysujPodstawy() const;
	void RysujScianeBoczna() const;
public:
	Walec(GLfloat x, GLfloat y, GLfloat z, GLfloat r, GLfloat h, Axis orientacja = Axis::X);
	void Render(bool czyWypelnic = true) const override;
	void UstawPrecyzje(GLfloat precyzja);
	void UstawKolor(GLfloat r, GLfloat g, GLfloat b);
	void UstawKolor(const GLfloat* v);
	void UstawRysowaniePodstaw(bool czyRysowac);
};

