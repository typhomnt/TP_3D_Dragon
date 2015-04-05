#include <iostream>
using namespace std;
#include "trapezeIsocele.h"
#include <stdexcept>

/*
                Y
                |
            2 - - - - 3
           /           \
  z <--   /             \
         /               \
        1 - - - - - - - - 0
              /
             X
*/
TrapezeIsocele::TrapezeIsocele(double bigBase, double smallBase, double height, double depth) {
    if (bigBase <= 0 || smallBase <= 0 || height <= 0 || depth <= 0)
        throw std::invalid_argument("bigbase, smallbase and height must be positive");
    if (smallBase > bigBase)
        throw std::invalid_argument("smallbase must be smaller than bigbase");
    this->bb = bigBase;
    this->sb = smallBase;
    this->h = height;
    this->d = depth;
}


void TrapezeIsocele::draw()
{
    glPushMatrix();
    drawImmediate();
    glPopMatrix();
}

void TrapezeIsocele::drawImmediate()
{
    GLfloat s0[] = {-d/2.0, 0.0, 0.0};
    GLfloat s1[] = {s0[0], 0.0, bb};
    GLfloat s2[] = {s0[0], h, (bb-sb)/2.0 + sb};
    GLfloat s3[] = {s0[0], h, (bb-sb)/2.0};
    GLfloat s4[] = {-s0[0], s0[1], s0[2]};
    GLfloat s5[] = {-s0[0], s1[1], s1[2]};
    GLfloat s6[] = {-s0[0], s2[1], s2[2]};
    GLfloat s7[] = {-s0[0], s3[1], s3[2]};

    glBegin(GL_QUADS);


        // 0 3 2 1
        glNormal3f(-1.0, 0.0, 0.0);
        glVertex3fv(s0);
        glVertex3fv(s1);
        glVertex3fv(s2);
        glVertex3fv(s3);

        // 0 4 5 1
        glNormal3f(0.0, -1.0, 0.0);
        glVertex3fv(s0);
        glVertex3fv(s4);
        glVertex3fv(s5);
        glVertex3fv(s1);

        // 4 7 6 5
        glNormal3f(1.0, 0.0, 0.0);
        glVertex3fv(s4);
        glVertex3fv(s7);
        glVertex3fv(s6);
        glVertex3fv(s5);

        // 3 2 6 7
        glNormal3f(0.0, 1.0, 0.0);
        glVertex3fv(s3);
        glVertex3fv(s2);
        glVertex3fv(s6);
        glVertex3fv(s7);

        // 0 3 7 4
        glNormal3f(0.0, (bb-sb)/(2.0*h), -1.0);
        glVertex3fv(s0);
        glVertex3fv(s3);
        glVertex3fv(s7);
        glVertex3fv(s4);

        // 6 2 1 5
        glNormal3f(0.0, (bb-sb)/(2.0*h), 1.0);
        glVertex3fv(s6);
        glVertex3fv(s2);
        glVertex3fv(s1);
        glVertex3fv(s5);

    glEnd();
}




