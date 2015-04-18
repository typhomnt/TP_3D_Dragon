#include <iostream>
#include <cstdio>
#ifndef __APPLE__
	#include <GL/glew.h>
	#include <GL/glut.h>
#else
	#include <GLUT/glut.h>
#endif
#include <QGLViewer/qglviewer.h>

#include "particule.h"


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

void Particule::incrColor(qglviewer::Vec incr) {
	this->color += incr;
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
qglviewer::Vec Particule::getGravity() {
	return gravity;
}

void Particule::setGravity(qglviewer::Vec gravity) {
	this->gravity = gravity;
}