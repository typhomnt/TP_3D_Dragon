#ifndef DRAGON_H
#define DRAGON_H
#include "renderable.h"
#include "cylinder.h"

#ifndef __APPLE__
#include <GL/glut.h>
#else
#include <GLUT/glut.h>
#endif


class Dragon : public Renderable
{
public:
    Dragon();
    ~Dragon();
    void draw();
    void init(Viewer &v);

private :
    void drawBody();
    void drawTail();
    Cylinder *c;

};

#endif // DRAGON_H
