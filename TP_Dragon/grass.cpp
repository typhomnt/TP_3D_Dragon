#include <iostream>
#include <cstdio>
#include <cmath>
#ifndef __APPLE__
	#include <GL/glew.h>
	#include <GL/glut.h>
#else
	#include <GLUT/glut.h>
#endif
#include <QGLViewer/qglviewer.h>

#include "grass.h"


// Couleur vert gazon
static qglviewer::Vec gazon(58.0/255.0, 157.0/255.0, 35.0/255.0);


///////////////////////////////////////////////////////////////////////////////
Grass::Grass(double size, int nbParticles, int iter) {
	this->size = size;
	this->iter = iter;
	this->base = std::vector<Particule>(nbParticles);
	this->elements = std::vector<Particule>();
}


///////////////////////////////////////////////////////////////////////////////
void Grass::init(Viewer &v) {
	(void)v;

	initParticles();

	// On ajoute les sphères initiales au vecteur d'affichage
	for (unsigned int i = 0; i < base.size(); i++)
		elements.push_back(Particule(base[i]));

	for (int j = 0; j < this->iter; j++) {
		for (unsigned int i = 0; i < base.size(); i++) {
			Particule &p = base[i];
			p.incrPos(p.getMovVec() / 200);
			p.incrLife(-p.getVelDis());

			elements.push_back(Particule(p));
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
void Grass::draw() {
	double radius = 0.02;

	glColor3f(gazon[0], gazon[1], gazon[2]);
	for (unsigned int i = 0; i < elements.size(); i++) {
		qglviewer::Vec pos = elements[i].getPos();

		glPushMatrix();
		glTranslatef(pos[0], pos[1], pos[2] - 5.0);
		//glutSolidSphere(radius, 5, 5);
		glutSolidCube(radius);
		glPopMatrix();
		
		if (i % base.size() == 0 && i != 0)
			radius -= 0.001;
	}
}


///////////////////////////////////////////////////////////////////////////////
double Grass::alea(double min, double max) {  
	return ((rand() / (double)RAND_MAX) * (max - min)) + min;
}


///////////////////////////////////////////////////////////////////////////////
void Grass::initParticles() {
	for (unsigned int i = 0; i < base.size(); i++) {
		Particule &p = base[i];

		p.setActive(true);
		p.setLife(1.0);
		p.setVelDis(1.0 / (double)iter);
		p.setColor(gazon);

		// Position générée aléatoirement
		double x = alea(-size, size);
		double y = alea(-size, size);
		p.setPos(qglviewer::Vec(x, y, 0));

		// L'angle du vecteur de mouvement ne doit pas être trop fort
		double vx = alea(-M_PI/4.0, M_PI/4.0);
		double vy = alea(-M_PI/4.0, M_PI/4.0);
		p.setMovVec(qglviewer::Vec(vx, vy, 1));

		p.setGravity(qglviewer::Vec());
	}
} 


