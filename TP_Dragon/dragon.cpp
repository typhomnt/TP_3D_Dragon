#include "dragon.h"
#include <iostream>

#define T_GLOBALE 1.0
#define T_TAIL 1

using namespace std ;

Dragon::Dragon()
{
        c = new Cylinder();
}

Dragon::~Dragon(){
    delete c;
}

void Dragon::draw(){

    glPushMatrix();

    //draw the body
    //drawBody();
    //glRotatef(M_PI/2,1.0,0.0,0.0);
    drawTail();
    glPopMatrix();

}

void Dragon::drawTail(){
    /*glEnable(GL_FOG);
    GLfloat fog[4] = {0.5,0.5,0.5,1};
    glFogi(GL_FOG_MODE, GL_EXP);
    glFogfv(GL_FOG_COLOR, fog);
    glFogf(GL_FOG_DENSITY, 0.35);*/
    glScalef(T_TAIL,T_TAIL ,T_TAIL);
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
    glTranslatef(0,0,1);
    glutSolidSphere(0.4,100,100);
}
