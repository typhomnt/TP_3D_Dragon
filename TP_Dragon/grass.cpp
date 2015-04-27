#include <iostream>
#include <cstdio>
#include <cmath>
#include <cassert>
#ifndef __APPLE__
	#include <GL/glew.h>
	#include <GL/glut.h>
#else
	#include <GLUT/glut.h>
#endif
#include <QGLViewer/qglviewer.h>

#include "grass.h"

static qglviewer::Vec gazon(0.0, 255.0, 0);

///////////////////////////////////////////////////////////////////////////////
Grass::Grass(double size, int nbParticles, int iter) {
	this->size = size;
	this->iter = iter;
	this->base = std::vector<Particule>(nbParticles);
	this->elements = std::vector<Particule>();

	this->radius = 0.02;
	this->radiusStep = this->radius / this->iter;
	this->zg = -0.1;
}


///////////////////////////////////////////////////////////////////////////////
void Grass::init(Viewer &v) {
	(void)v;

	initParticles();

	// On ajoute les sphères initiales au vecteur d'affichage
	for (unsigned int i = 0; i < base.size(); i++)
		elements.push_back(Particule(base[i]));

    /**
     * On génère toutes les particules dans l'initialisation
     * On les dessine après; ce qui fait que l'herbe ne bouge pas dans notre cas
     */
	for (int j = 0; j < this->iter; j++) {
		for (unsigned int i = 0; i < base.size(); i++) {
			Particule &p = base[i];
			p.incrPos(p.getMovVec() / 50);
			p.incrLife(-p.getVelDis());
			p.incrMovVec(p.getGravity());

			elements.push_back(Particule(p));
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
void Grass::draw() {
	double actRadius = this->radius;

	glColor3f(gazon[0], gazon[1], gazon[2]);
	for (unsigned int i = 0; i < elements.size(); i++) {
		qglviewer::Vec pos = elements[i].getPos();

		glPushMatrix();
        glTranslatef(pos[0] - 20, pos[1] - 20, pos[2]);
        glutSolidCube(actRadius);
		glPopMatrix();
		
        // On diminue le rayon des cubes à chaque étape de génération
		if (i % base.size() == 0 && i != 0)
			actRadius -= radiusStep;
	}
}


///////////////////////////////////////////////////////////////////////////////
void Grass::setRadius(double radius) {
	assert(radius > 0.0);
	this->radius = radius;
	this->radiusStep = this->radius / this->iter;
}


///////////////////////////////////////////////////////////////////////////////
void Grass::setZg(double zg) {
	this->zg = zg;
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
		double vx = alea(-M_PI/12.0, M_PI/12.0);
		double vy = alea(-M_PI/12.0, M_PI/12.0);
		p.setMovVec(qglviewer::Vec(vx, vy, 1));

		p.setGravity(qglviewer::Vec(0.0, 0.0, zg));
	}
} 


