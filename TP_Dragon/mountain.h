#ifndef MOUNTAIN_H
#define MOUNTAIN_H
#include "renderable.h"
#include <QGLViewer/qglviewer.h>
class Mountain : public Renderable
{
public:
    Mountain(float size, float slice, qglviewer::Vec pos,bool ice=false);
    void build();
    void draw();
    void animate();
private:
     float size;
     float slice;
     qglviewer::Vec** ground;
     qglviewer::Vec pos;
     float interpol(float beg, float end, float diff, int step);
     float **colArr;
     bool ice;
};


#endif // MOUNTAIN_H
