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
     float slicex;
     float slicey;
     int step2D;
     int oct2D;
     int max_size;
     qglviewer::Vec* ground;
     qglviewer::Vec pos;
     float interpol(float beg, float end, float diff, int step);
     void initNoise(int size, int step, int oct);
     float noise2D(int i, int j);
     float interpol_cos2D(float a, float b, float c, float d, float x, float y);
     float noise2D_function(float x, float y);
     float perlin(float x, float y, float rough);

};


#endif // MOUNTAIN_H
