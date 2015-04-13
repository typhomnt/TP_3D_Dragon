
#ifndef __APPLE__
#include <GL/glew.h>
#include <GL/glut.h>
#else
#include <GLUT/glut.h>
#endif
#include "spring.h"


Spring::Spring(const  Sphere *s1, const Sphere *s2,
           double s, double l0, double d)
    : s1(s1), s2(s2),
    stiffness(s),equilibriumLength(l0), damping(d)
{
}


qglviewer::Vec Spring::getCurrentForce() const
{
    // we compute the force applied on particle 1 by particle 2

    qglviewer::Vec u = s1->getPosition() - s2->getPosition();

    double uNorm = u.normalize();	// u is thereafter normalized!
    if (uNorm < 1.0e-6)
        return qglviewer::Vec();	// null force

    // force due to stiffness only
    qglviewer::Vec sF = -stiffness * (uNorm - equilibriumLength) * u;
    // damping force
    qglviewer::Vec dF = -damping * ((s1->getVelocity() - s2->getVelocity()) * u) * u;

    return sF + dF;
}

const Sphere *Spring::getParticle1() const
{
    return s1;
}

const Sphere *Spring::getParticle2() const
{
    return s2;
}


void Spring::draw() const
{
    glBegin(GL_LINES);
    qglviewer::Vec pos1 = s1->getPosition();
    qglviewer::Vec pos2 = s2->getPosition();
    glVertex3f(pos1.x, pos1.y, pos1.z);
    glVertex3f(pos2.x, pos2.y, pos2.z);
    glEnd();
}

