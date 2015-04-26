#include <iostream>
#include <cmath>
#ifndef __APPLE__
    #include <GL/glew.h>
    #include <GL/glut.h>
#else
    #include <GLUT/glut.h>
#endif

#include "tooth.h"

Tooth::Tooth(qglviewer::Vec &v1, qglviewer::Vec &v2, qglviewer::Vec &v3, qglviewer::Vec &v4, qglviewer::Vec &v5)
{
    this->v1 = v1;
    this->v2 = v2;
    this->v3 = v3;
    this->v4 = v4;
    this->v5 = v5;
}

void Tooth::draw() {
    glPushMatrix();
    float nx = xNormal(v1,v2,v5);
    float ny = yNormal(v1,v2,v5);
    float nz = zNormal(v1,v2,v5);
    float normeN = norme(nx,ny,nz);
    nx = nx/normeN;
    ny = ny/normeN;
    nz = nz/normeN;

    glBegin(GL_TRIANGLES);
    glNormal3f(nx,ny,nz);
    glVertex3fv(v1);
    glVertex3fv(v2);
    glVertex3fv(v5);
    glEnd();

    nx = xNormal(v2,v3,v5);
    ny = yNormal(v2,v3,v5);
    nz = zNormal(v2,v3,v5);
    normeN = norme(nx,ny,nz);
    nx = nx/normeN;
    ny = ny/normeN;
    nz = nz/normeN;

    glBegin(GL_TRIANGLES);
    glNormal3f(nx,ny,nz);
    glVertex3fv(v2);
    glVertex3fv(v3);
    glVertex3fv(v5);
    glEnd();

    nx = xNormal(v3,v4,v5);
    ny = yNormal(v3,v4,v5);
    nz = zNormal(v3,v4,v5);
    normeN = norme(nx,ny,nz);
    nx = nx/normeN;
    ny = ny/normeN;
    nz = nz/normeN;

    glBegin(GL_TRIANGLES);
    glNormal3f(nx,ny,nz);
    glVertex3fv(v3);
    glVertex3fv(v4);
    glVertex3fv(v5);
    glEnd();

    nx = xNormal(v4,v1,v5);
    ny = yNormal(v4,v1,v5);
    nz = zNormal(v4,v1,v5);
    normeN = norme(nx,ny,nz);
    nx = nx/normeN;
    ny = ny/normeN;
    nz = nz/normeN;

    glBegin(GL_TRIANGLES);
    glNormal3f(nx,ny,nz);
    glVertex3fv(v4);
    glVertex3fv(v1);
    glVertex3fv(v5);
    glEnd();
}

float Tooth::norme(float x, float y, float z) {
    return sqrt(x*x + y*y + z*z);
}

float Tooth::xNormal(qglviewer::Vec &v1, qglviewer::Vec &v2, qglviewer::Vec &v3) {
    float dy1 = v2.y-v1.y;
    float dz1 = v2.z-v1.z;

    float dy2 = v3.y-v1.y;
    float dz2 = v3.z-v1.z;

    return dy1*dz2 - dz1*dy2;
}

float Tooth::yNormal(qglviewer::Vec &v1, qglviewer::Vec &v2, qglviewer::Vec &v3) {
    float dx1 = v2[0]-v1[0];
    float dz1 = v2.z-v1.z;

    float dx2 = v3[0]-v1[0];
    float dz2 = v3.z-v1.z;

    return -(dx1*dz2 - dz1*dx2);
}

float Tooth::zNormal(qglviewer::Vec &v1, qglviewer::Vec &v2, qglviewer::Vec &v3) {
    float dx1 = v2[0]-v1[0];
    float dy1 = v2.y-v1.y;

    float dx2 = v3[0]-v1[0];
    float dy2 = v3.y-v1.y;

    return dx1*dy2 - dy1*dx2;
}

