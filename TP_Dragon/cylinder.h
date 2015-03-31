#ifndef _CYLINDER_
#define _CYLINDER_

#include "renderable.h"
#ifndef __APPLE__
#include <GL/glut.h>
#else
#include <GLUT/glut.h>
#endif

class Cylinder : public Renderable
{
 public :
    void draw();
    //Cylinder(GLfloat r, GLfloat h, int s);
    //~Cylinder();
    //void init(Viewer & v);

 private:
    void drawImmediate();
    void drawElements();
    void drawArrays();
    GLfloat radius;
    GLfloat height;
    int step;
    GLfloat *vertex;
    GLfloat *normal;
};
#endif
