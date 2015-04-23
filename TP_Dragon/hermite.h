#ifndef DEF_HERMITE
#define DEF_HERMITE

#include <vector>
#include <QGLViewer/qglviewer.h>

using std::vector;
using qglviewer::Vec;

class Hermite {

    public:
        static vector<Vec> generate(vector<Vec> ctlPts, double step);

    private:
        static void casteljau(vector<Vec> &bezier, double step,
                                            vector<Vec> &hermite);

        static Vec getPoint(vector<Vec> &bezier, int r, int i, double t);

};

#endif
