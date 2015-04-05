#ifndef TRAPEZEISOCELE_H
#define TRAPEZEISOCELE_H

#include "renderable.h"
#ifndef __APPLE__
#include <GL/glut.h>
#else
#include <GLUT/glut.h>
#endif

class TrapezeIsocele : public Renderable
{
public:
    TrapezeIsocele(double bigBase, double smallBase, double height, double depth);
    void draw();

private:
    // Big base on the Z axis
    double bb;

    // Small base on the Z axis
    double sb;

    // Height on the Y axis
    double h;

    // Depth on the X axis
    double d;

    void drawImmediate();
};

#endif // TRAPEZEISOCELE_H
