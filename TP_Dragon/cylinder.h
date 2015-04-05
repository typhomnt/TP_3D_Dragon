#ifndef CYLINDER_H
#define CYLINDER_H

#include "renderable.h"


class Cylinder : public Renderable {

public:
    Cylinder(double height = 1, double radius = 1, int step = 60);
    ~Cylinder();

    void init(Viewer &v);
    void draw();

    void updateTexture(GLint texture0, GLint texcoord0);

private:
    void drawImmediate();
    void drawArrays();

    double h;     // Height
    double r;     // Radius
    int step;  // Number of steps
    GLfloat *allVertices;  // Vertices
    GLfloat *allNormals;   // Normals

    GLint texture0;
    GLint texcoord0;
};


#endif // CYLINDER_H
