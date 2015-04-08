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

    this->center_x = 0;
    this->center_y = 0;
    this->center_z = 0;

    this->allNormals  = new GLfloat[18*step + 30];
    this->allVertices = new GLfloat[18*step + 30];

    this->texture0 = 0;
    this->texcoord0 = 0;
}

Cylinder::~Cylinder() {
    delete[] this->allNormals;
    delete[] this->allVertices;
}

void Cylinder::setCenter(double x, double y ,double z){
    this->center_x = x;
    this->center_y = y;
    this->center_z = z;

}

 double Cylinder::getx(){
     return this->center_x;
 }

 double Cylinder::gety(){
     return this->center_y;
 }

 double Cylinder::getz(){
     return this->center_z;
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
    for (int i = 6*step+12; i <= 18*step+12; i+=12) {
        allNormals[i] = allNormals[i+3] = cos(j*ANGLE);
        allNormals[i+1] = allNormals[i+4] = sin(j*ANGLE);
        allNormals[i+2] = allNormals[i+5] = 0;

        allNormals[i+6] = allNormals[i+9] = cos((j+1)*ANGLE);
        allNormals[i+7] = allNormals[i+10] = sin((j+1)*ANGLE);
        allNormals[i+8] = allNormals[i+11] = 0;

        allVertices[i] = allVertices[i+3] = r * cos(j*ANGLE);
        allVertices[i+1] = allVertices[i+4] = r * sin(j*ANGLE);
        allVertices[i+2] = 0;
        allVertices[i+5] = h;

        allVertices[i+6] = allVertices[i+9] = r * cos((j+1)*ANGLE);
        allVertices[i+7] = allVertices[i+10] = r * sin((j+1)*ANGLE);
        allVertices[i+11] = 0;
        allVertices[i+8] = h;

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
    glBegin(GL_QUADS);
        for (int i = 6*step+12; i <= 18*step+12; i+=12) {
            glNormal3f(allNormals[i], allNormals[i+1], allNormals[i+2]);
            glVertexAttrib2f(texcoord0, 0, 0);
            glVertex3f(allVertices[i], allVertices[i+1], allVertices[i+2]);
            glVertexAttrib2f(texcoord0, 1, 0);
            glVertex3f(allVertices[i+3], allVertices[i+4], allVertices[i+5]);
            glVertexAttrib2f(texcoord0, 1, 1);
            glVertex3f(allVertices[i+6], allVertices[i+7], allVertices[i+8]);
            glVertexAttrib2f(texcoord0, 0, 1);
            glVertex3f(allVertices[i+9], allVertices[i+10], allVertices[i+11]);
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














