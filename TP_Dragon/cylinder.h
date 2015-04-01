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
    Cylinder(int height = 1, int radius = 1, int step = 60);
    ~Cylinder();

    void init(Viewer &v);
    void draw();

private:
    void drawImmediate();
    void drawArrays();

    int h;     // Height
    int r;     // Radius
    int step;  // Number of steps
    GLfloat *allVertices;  // Vertices
    GLfloat *allNormals;   // Normals
};


#endif // CYLINDER_H
