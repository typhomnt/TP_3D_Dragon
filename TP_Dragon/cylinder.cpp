#include <iostream>
#include <cmath>
#ifndef __APPLE__
    #include <GL/glew.h>
    #include <GL/glut.h>
#else
    #include <GLUT/glut.h>
#endif

#include "cylinder.h"

/* *** CONSTRUCTOR - DESTRUCTOR *** */
Cylinder::Cylinder(double height, double radius, int step) {
    this->h = height;
    this->r = radius;
    this->step = step;

    this->allNormals = new GLfloat[12*step+17];
    this->allVertices = new GLfloat[12*step+17];

    this->texture0 = 0;
    this->texcoord0 = 0;
}

Cylinder::~Cylinder() {
    delete[] this->allNormals;
    delete[] this->allVertices;
}


/* *** METHODS *** */
void Cylinder::init(Viewer &v) {
    const float ANGLE = 2.0 * M_PI / (float)step;
    int i = 0, j = 0;

    // Lower face
    allVertices[0] = allVertices[1] = allVertices[2] = 0.0;
    allNormals[0] = allNormals[1] = 0.0;
    allNormals[2] = -1.0;

    for (int i = 3; i <= 3*step+3; i += 3) {
        allNormals[i] = allNormals[i+1] = 0.0;
        allNormals[i+2] = -1.0;
        allVertices[i] = r * cos(j*ANGLE);
        allVertices[i+1] = r * sin(j*ANGLE);
        allVertices[i+2] = 0.0;
        j++;
    }

    // Upper face
    i = 3*step + 6;
    allVertices[i] = allVertices[i+1] = 0.0;
    allVertices[i+2] = h;
    allNormals[i] = allNormals[i+1] = 0.0;
    allNormals[i+2] = 1.0;

    j = 0;
    for (int i = 3*step+9; i <= 6*step+9; i += 3) {
        allNormals[i] = allNormals[i+1] = 0.0;
        allNormals[i+2] = 1.0;
        allVertices[i] = r * cos(j*ANGLE);
        allVertices[i+1] = r * sin(j*ANGLE);
        allVertices[i+2] = h;
        j++;
    }

    // Lateral face
    j = 0;
    for (int i = 6*step+12; i <= 12*step+12; i+=6) {
        allNormals[i] = allNormals[i+3] = cos(j*ANGLE);
        allNormals[i+1] = allNormals[i+4] = sin(j*ANGLE);
        allNormals[i+2] = allNormals[i+5] = 0;

        allVertices[i] = allVertices[i+3] = r * cos(j*ANGLE);
        allVertices[i+1] = allVertices[i+4] = r * sin(j*ANGLE);
        allVertices[i+2] = 0;
        allVertices[i+5] = h;

        j++;
    }
}

void Cylinder::draw() {
    glPushMatrix();
    drawImmediate();
    //drawArrays();
    glPopMatrix();
}

void Cylinder::updateTexture(GLint texture0, GLint texcoord0) {
    this->texture0 = texture0;
    this->texcoord0 = texcoord0;
}



/* *** PRIVATE METHODS *** */
void Cylinder::drawImmediate() {
    // Lower face
    glBegin(GL_TRIANGLE_FAN);
        glNormal3f(allNormals[0], allNormals[1], allNormals[2]);
        for (int i = 0; i <= 3*step+3; i+=3)
            glVertex3f(allVertices[i], allVertices[i+1], allVertices[i+2]);
    glEnd();

    // Upper face
    glBegin(GL_TRIANGLE_FAN);
        glNormal3f(allNormals[3*step+6], allNormals[3*step+7], allNormals[3*step+8]);
        for (int i = 3*step+9; i <= 6*step+9; i += 3)
            glVertex3f(allVertices[i], allVertices[i+1], allVertices[i+2]);
    glEnd();

    // Lateral face
    glBegin(GL_QUAD_STRIP);
        for (int i = 6*step+12; i <= 12*step+12; i+=6) {
            glVertexAttrib2f(texcoord0, 0, 0);
            glNormal3f(allNormals[i], allNormals[i+1], allNormals[i+2]);
            glVertex3f(allVertices[i], allVertices[i+1], allVertices[i+2]);
            glVertex3f(allVertices[i+3], allVertices[i+4], allVertices[i+5]);
        }
    glEnd();
}

void Cylinder::drawArrays() {
    // activate the use of GL_VERTEX_ARRAY and GL_NORMAL_ARRAY
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);

    // specify the arrays to use
    glNormalPointer(GL_FLOAT, 0, allNormals);
    glVertexPointer(3, GL_FLOAT, 0 , allVertices);

    // draw quads using 24 data stored in arrays, starting at index 0
    glDrawArrays(GL_TRIANGLE_FAN, 0, step+2);
    glDrawArrays(GL_TRIANGLE_FAN, step+3, 2*step+5);
    glDrawArrays(GL_QUADS, 2*step+6, 6*step+8);

    // disable the use of arrays (do not forget!)
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

}














