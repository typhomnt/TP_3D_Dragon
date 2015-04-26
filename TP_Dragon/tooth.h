#ifndef TOOTH_H
#define TOOTH_H

#include <QGLViewer/qglviewer.h>
#include "renderable.h"

class Tooth : public Renderable
{
public:
    Tooth(qglviewer::Vec &v1, qglviewer::Vec &v2, qglviewer::Vec &v3, qglviewer::Vec &v4, qglviewer::Vec &v5);
    void draw();
    qglviewer::Vec getV1() const;
    qglviewer::Vec getV2() const;
    qglviewer::Vec getV3() const;
    qglviewer::Vec getV4() const;
    qglviewer::Vec getV5() const;
    void setV1(qglviewer::Vec pos);
    void setV2(qglviewer::Vec pos);
    void setV3(qglviewer::Vec pos);
    void setV4(qglviewer::Vec pos);
    void setV5(qglviewer::Vec pos);
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
