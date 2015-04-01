#ifndef CYLINDER_H
#define CYLINDER_H

#include "renderable.h"
#ifndef __APPLE__
#include <GL/glut.h>
#else
#include <GLUT/glut.h>
#endif


class Cylinder : public Renderable {

public:
    Cylinder(double height = 1, double radius = 1, int step = 60);
    ~Cylinder();

    void init(Viewer &v);
    void draw();

private:
    void drawImmediate();
    void drawArrays();

    double h;     // Height
    double r;     // Radius
    int step;  // Number of steps
    GLfloat *allVertices;  // Vertices
    GLfloat *allNormals;   // Normals
};


#endif // CYLINDER_H
