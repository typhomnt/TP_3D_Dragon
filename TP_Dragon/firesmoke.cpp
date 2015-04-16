#include <iostream>
#include <cstdio>
#ifndef __APPLE__
	#include <GL/glew.h>
	#include <GL/glut.h>
#else
	#include <GLUT/glut.h>
#endif
#include <QGLViewer/qglviewer.h>

#include "firesmoke.h"


///////////////////////////////////////////////////////////////////////////////
// CLASSE PARTICULE
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
bool Particule::isActive() {
	return active;
}

void Particule::setActive(bool active) {
	this->active = active;
}


///////////////////////////////////////////////////////////////////////////////
double Particule::getLife() {
	return life;
}

void Particule::setLife(double life) {
	this->life = life;
}

void Particule::incrLife(double incr) {
	this->life += incr;
}


///////////////////////////////////////////////////////////////////////////////
double Particule::getVelDis() {
	return velDis;
}

void Particule::setVelDis(double velDis) {
	this->velDis = velDis;
} 


///////////////////////////////////////////////////////////////////////////////
qglviewer::Vec Particule::getColor() {
	return color;
}

void Particule::setColor(qglviewer::Vec color) {
	this->color = color;
}


///////////////////////////////////////////////////////////////////////////////
qglviewer::Vec Particule::getPos() {
	return pos;
}
		
void Particule::setPos(qglviewer::Vec pos) {
	this->pos = pos;
}

void Particule::incrPos(qglviewer::Vec incr) {
	this->pos += incr;
}


///////////////////////////////////////////////////////////////////////////////
qglviewer::Vec Particule::getMovVec() {
	return movVec;
}

void Particule::setMovVec(qglviewer::Vec movVec) {
	this->movVec = movVec;
}

void Particule::incrMovVec(qglviewer::Vec incr) {
	this->movVec += incr;
}




///////////////////////////////////////////////////////////////////////////////
/// CLASSE FIRESMOKE 
///////////////////////////////////////////////////////////////////////////////

FireSmoke::FireSmoke(bool firesmoke, qglviewer::Vec origin, int nbParticles) {
	this->firesmoke = firesmoke;
	this->origin = origin;
	this->particles = std::vector<Particule>(nbParticles);
	this->nbParticles = nbParticles;
	this->active = false;
	this->inactivateReq = false;
}


///////////////////////////////////////////////////////////////////////////////
void FireSmoke::init(Viewer &v) {
	(void)v;

	for (unsigned int i = 0; i < particles.size(); i++)
		initParticle(particles[i]);
}


///////////////////////////////////////////////////////////////////////////////
void FireSmoke::draw() {
	const double SIZE = 0.02;

	for (unsigned int i = 0; i < particles.size(); i++) {
		if (particles[i].isActive()) {
			qglviewer::Vec color = particles[i].getColor();
			qglviewer::Vec pos = particles[i].getPos();

			glColor4d(color[0], color[1], color[2], particles[i].getLife());
			glBegin(GL_TRIANGLE_STRIP);
              	glVertex3d(pos[0] + SIZE, pos[1], pos[2] + SIZE);
              	glVertex3d(pos[0] - SIZE, pos[1], pos[2] + SIZE);
              	glVertex3d(pos[0] + SIZE, pos[1], pos[2] - SIZE);
              	glVertex3d(pos[0] - SIZE, pos[1], pos[2] - SIZE);
            glEnd();
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
void FireSmoke::animate() {
	for (unsigned int i = 0; i < particles.size(); i++) {
		if (particles[i].isActive()) {
			Particule &p = particles[i];
			qglviewer::Vec mov = particles[i].getMovVec();

			p.incrPos(mov / 12);
			p.incrLife(-p.getVelDis());

			if (p.getLife() < 0) {
				if (inactivateReq) {
					p.setActive(false);
					this->nbParticles--;
					if (nbParticles == 0)
						active = false;
				}
				else {
					initParticle(particles[i]);
				}
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
qglviewer::Vec FireSmoke::getOrigin() {
	return origin;
}


///////////////////////////////////////////////////////////////////////////////
void FireSmoke::setOrigin(qglviewer::Vec origin) {
	this->origin = origin;
}


///////////////////////////////////////////////////////////////////////////////
bool FireSmoke::isActive() {
	return active;
}


///////////////////////////////////////////////////////////////////////////////
void FireSmoke::activate() {
	this->active = true;
	this->inactivateReq = false;
	this->nbParticles = particles.size();
	for (unsigned int i = 0; i < particles.size(); i++)
		initParticle(particles[i]);
}


///////////////////////////////////////////////////////////////////////////////
void FireSmoke::inactivate() {
	this->inactivateReq = true;
}


///////////////////////////////////////////////////////////////////////////////
void FireSmoke::initParticle(Particule &p) {
	p.setActive(true);
	p.setLife(1.0);
	p.setVelDis(alea(0.03, 0.06));

	if (firesmoke)
		p.setColor(qglviewer::Vec(1, 0, 0));
	else
		p.setColor(qglviewer::Vec(127.0/255.0, 127.0/255.0, 127.0/255.0));

	p.setPos(origin);
	p.setMovVec(qglviewer::Vec(alea(0.5,1), alea(0.5,1), alea(0.5,1)));
}



///////////////////////////////////////////////////////////////////////////////
double FireSmoke::alea(double min, double max) {  
	return (double) (min + ((float) rand() / RAND_MAX * (max - min + 1.0)));
} 


