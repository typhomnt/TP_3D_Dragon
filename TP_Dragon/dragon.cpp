#include "dragon.h"
#include <iostream>

#define T_GLOBALE 1
#define T_TAIL 0.1

using namespace std ;

Dragon::Dragon()
{
        c = new Cylinder(2,0.5,60);
}

Dragon::~Dragon(){
    delete c;
}

void Dragon::init(Viewer &v) {
    c->init(v);
}

void Dragon::draw(){

    glPushMatrix();

    //draw the body
    //drawBody();
    //glRotatef(M_PI/2,1.0,0.0,0.0);
    drawTail();
    c->draw();
    glPopMatrix();

}

void Dragon::drawTail(){
    /*glEnable(GL_FOG);
    GLfloat fog[4] = {0.5,0.5,0.5,1};
    glFogi(GL_FOG_MODE, GL_EXP);
    glFogfv(GL_FOG_COLOR, fog);
    glFogf(GL_FOG_DENSITY, 0.35);*/
    // Corps
    glScalef(T_TAIL,T_TAIL ,T_TAIL);
    glScalef(0.5,0.5,1.0);
    //glutSolidSphere(4,100,100);
    glScalef(2.0,2.0,1.0);
    glPushMatrix();

    //Queue
    glTranslatef(0,0,4.5);
    glutSolidSphere(0.2,100,100);
    glTranslatef(0,0,0.5);
    c->draw();
    glTranslatef(0,0,2.5);
    glutSolidSphere(0.2, 100, 100);
    glTranslatef(-0.25,0,0.5);
    glRotatef(-30,0,1,0);
    c->draw();
    glTranslatef(0,0,2.5);
    glutSolidSphere(0.2, 100, 100);
    glTranslatef(0,0,0.5);
    c->draw();

    glPushMatrix();
    glTranslatef(0,-0.25,2.3) ;
    glRotatef(45,1.0,0.0,0.0);
    glScalef(0.5,0.5,1.0);
    c->draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,0.25,2.3);
    glRotatef(-45,1.0,0.0,0.0);
    glScalef(0.5,0.5,1.0);
    c->draw();
    glPopMatrix();

    glTranslatef(0,0,2);
    glutSolidSphere(0.4,100,100);
    //glPopMatrix();
    //glPopMatrix();

    // Tete
    /*glPushMatrix();
    glScalef(T_TAIL,T_TAIL ,T_TAIL);
    glTranslatef(0,0,-4.5);
    glutSolidSphere(0.1,100,100);
    glTranslatef(0,0,-0.5);
    c->draw();
    glTranslatef(0,0,-2.5);
    glutSolidSphere(0.2, 100, 100);
    glTranslatef(0.25,0,-0.5);
    glRotatef(-30,0,1,0);
    c->draw();
    glTranslatef(0,0,-2.5);
    glutSolidSphere(0.2, 100, 100);
    glTranslatef(0,0,-0.5);
    c->draw();*/
    /*glScalef(T_TAIL,T_TAIL ,T_TAIL);
    c->draw();
    glPushMatrix();

    glTranslatef(0 ,-0.8,1.9) ;
    glRotatef(45,1.0,0.0,0.0);
    glScalef(0.5,0.5,1.0);
    c->draw();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0,0.8,1.9);
    glRotatef(-45,1.0,0.0,0.0);
    glScalef(0.5,0.5,1.0);
    c->draw();
    glPopMatrix();
    glTranslatef(0,0,2);
    glutSolidSphere(0.4,100,100);*/
}
