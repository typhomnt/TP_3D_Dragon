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


FireSmoke::FireSmoke(bool firesmoke, qglviewer::Vec origin, int nbParticles, bool dust) {
	this->firesmoke = firesmoke;
	this->origin = origin;
	this->particles = std::vector<Particule>(nbParticles);
	this->nbParticles = nbParticles;
	this->active = false;
	this->inactivateReq = false;
    this->dust=dust;
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
            // Dessiner deux triangles est plus optimisé que dessiner un carré
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

            p.incrPos(mov / 20);
			p.incrLife(-p.getVelDis());

			double life = p.getLife();

            /**
             * On change la couleur de la particule en fonction de sa durée de vie
             * Cela permet de faire des "dégradés" de couleur, ce qui est plus
             * résliste. La durée de vie de la particule définit sa couleur
             */
			if (firesmoke) {
				if (life > 0.50 && life <= 0.75)
					p.setColor(qglviewer::Vec(255,255,0));
				else if (life > 0.25 && life <= 0.50)
					p.setColor(qglviewer::Vec(255, 2, 0));
				else
					p.setColor(qglviewer::Vec(255,0,0));
			}
			else if (!dust) {
				if (life > 0.50 && life <= 0.75)
					p.setColor(qglviewer::Vec(0, 0, 0));
				else if (life > 0.25 && life <= 0.50)
					p.setColor(qglviewer::Vec(2, 2, 2));
				else
                    p.setColor(qglviewer::Vec(0.5, 0.5, 0.5));
			}
			else {
                // Poussière de couleur marron
				p.setColor(qglviewer::Vec(2, 1, 0));
			}


            /**
             * Si la particule meurt:
             *   - soit on a demandé l'arrêt du feu, auquel cas on la désactive
             *     et on diminue le nombre de particules restantes
             *   - soit on la regénère, ce qui permet de faire un feu/fumée de
             *     manière "réaliste" et peu coûteuse
             */
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
	p.setVelDis(alea(0.01, 0.03));

    // On définit la couleur de base selon si on veut faire un feu ou une fumée
	if (firesmoke)
		p.setColor(qglviewer::Vec(255, 255, 255));
	else
		p.setColor(qglviewer::Vec(3, 3, 3));

    // Initialisation de la position et de la vitesse des particules
	p.setPos(origin);
    if(!dust)
        p.setMovVec(qglviewer::Vec(-alea(0.5,1), alea(-0.5,0.5), alea(0,0.5)));
    else{
        double angle = -alea(0,2*M_PI);
        double amort = 5*alea(0,1);
        p.setVelDis(alea(0.07, 0.09));
        p.setMovVec(qglviewer::Vec(amort*cos(angle), amort*sin(angle), alea(0,0.5)));
    }
}



///////////////////////////////////////////////////////////////////////////////
double FireSmoke::alea(double min, double max) {  
	return (double) (min + ((float) rand() / RAND_MAX * (max - min + 1.0)));
} 


