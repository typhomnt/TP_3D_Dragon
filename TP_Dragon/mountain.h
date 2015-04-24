#ifndef MOUNTAIN_H
#define MOUNTAIN_H
#include "renderable.h"
#include <QGLViewer/qglviewer.h>
class Mountain : public Renderable
{
public:
    Mountain(float size, float slice, qglviewer::Vec pos);
    void build();
    void draw();
private:
     float size;
     float slice;
     qglviewer::Vec** ground;
     qglviewer::Vec pos;
     float interpol(float beg, float end, float diff, int step);
     float **colArr;
};


#endif // MOUNTAIN_H
