#ifndef TOOTH_H
#define TOOTH_H

#include <QGLViewer/qglviewer.h>
#include "renderable.h"

class Tooth : public Renderable
{
public:
    Tooth(qglviewer::Vec &v1, qglviewer::Vec &v2, qglviewer::Vec &v3, qglviewer::Vec &v4, qglviewer::Vec &v5);
    void draw();

private:
    qglviewer::Vec v1;
    qglviewer::Vec v2;
    qglviewer::Vec v3;
    qglviewer::Vec v4;
    qglviewer::Vec v5;

    float norme(float x, float y, float z);
    float xNormal(qglviewer::Vec &v1, qglviewer::Vec &v2, qglviewer::Vec &v3);
    float yNormal(qglviewer::Vec &v1, qglviewer::Vec &v2, qglviewer::Vec &v3);
    float zNormal(qglviewer::Vec &v1, qglviewer::Vec &v2, qglviewer::Vec &v3);
};

#endif // TOOTH_H
