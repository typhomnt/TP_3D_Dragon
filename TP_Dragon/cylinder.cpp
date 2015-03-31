#include <iostream>
#include "math.h"
using namespace std;
#include "cylinder.h"

void Cylinder::draw()
{
	glPushMatrix();

	// draw immediate (center cube)
	drawImmediate();

    /*// draw arrays (left cube)
	glTranslatef(-2, 0, 0);
	drawArrays();

	// draw elements (right cube)
	glTranslatef(+4, 0, 0);
    drawElements();*/

	glPopMatrix();
}



//==================== 1. Immediate method ===================================
// immediate definition of individual vertex properties

void Cylinder::drawImmediate()
{
    float R = 0.5;
    GLfloat c0[] = {0.0,0.0,-1.0};
    GLfloat c1[] = {0.0,0.0,1.0};
    GLfloat v1[] = {0.0,R,1,0};
    GLfloat v2[] = {R*sin(2*M_PI/30),R*cos(2*M_PI/30),1.0};
    GLfloat v_temp[] = {0.0,R,1.0};
    GLfloat w1[] = {0.0,R,-1,0};
    GLfloat w2[] = {R*sin(2*M_PI/30),R*cos(2*M_PI/30),-1.0};

    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0,0.0,+1.0);
    glVertex3fv(v1);
    for(int i = 1 ; i <= 30 ; i++){
    v_temp = {R*sin(2*M_PI*i/30), R*cos(2*M_PI*i/30),1.0};
    glVertex3fv(v_temp);
    }
    glEnd();

    v_temp = {0.0,R,-1.0};

    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0,0.0,-1.0);
    glVertex3fv(w1);
    for(int i = 1 ; i <= 30 ; i++){
    v_temp = {R*sin(2*M_PI*i/30),R*cos(2*M_PI*i/30),-1.0};
    glVertex3fv(v_temp);
    }
    glEnd();
    glBegin(GL_QUAD_STRIP);
    for(int i = 1; i <= 31 ; i++){
    glNormal3f(sin(2*M_PI*(i - 1/2)/30), cos(2*M_PI*(i - 1/2)/30), 0.0);
    glVertex3f(R*sin(2*M_PI*(i-1)/30), R*cos(2*M_PI*(i-1)/30), 1.0);
    glVertex3f(R*sin(2*M_PI*(i-1)/30), R*cos(2*M_PI*(i-1)/30), -1.0);
    glVertex3fv(v2);
    glVertex3fv(w2);
    v2 = {R*sin(2*M_PI*i/30),  R*cos(2*M_PI*i/30), 1.0};
    w2 = {R*sin(2*M_PI*i/30),  R*cos(2*M_PI*i/30), -1.0};
    }
    glEnd();
}
void Cylinder::drawElements(){
}

void Cylinder::drawArrays(){
}



