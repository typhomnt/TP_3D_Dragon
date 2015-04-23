#include "hermite.h"

///////////////////////////////////////////////////////////////////////////////
vector<Vec> Hermite::generate(vector<Vec> ctlPts, double step) {
    const double C = 0.5;

    // Tangentes
    int size = ctlPts.size();
    vector<Vec> tangentes(size);
    tangentes[0] = ctlPts[1] - ctlPts[0];
    tangentes[size-1] = ctlPts[size-2] - ctlPts[size-1];

    for (int i = 1; i < size-1; i++)
        tangentes[i] = (1 - C) * 0.5 * (ctlPts[i+1] - ctlPts[i-1]);

    // Calcul de la courbe de hermite
    vector<Vec> hermite;
    for (int i = 0; i < size-1; i++) {
        vector<Vec> bezier(4);
        bezier[0] = ctlPts[i];
        bezier[3] = ctlPts[i+1];
        bezier[1] = ctlPts[i] + (1.0/3.0) * tangentes[i];
        bezier[2] = ctlPts[i] - (1.0/3.0) * tangentes[i+1];

        casteljau(bezier, step, hermite);
    }

    return hermite;
}


///////////////////////////////////////////////////////////////////////////////
void Hermite::casteljau(vector<Vec> &bezier, double step,
                                    vector<Vec> &hermite) {
    for (double t = 0; t <= 1; t += step) {
        Vec pt = getPoint(bezier, bezier.size()-1, 0, t);
        hermite.push_back(pt);
    }
}


///////////////////////////////////////////////////////////////////////////////
Vec Hermite::getPoint(vector<Vec> &bezier, int r, int i, double t) {
    if (r == 0)
        return bezier[i];

    Vec p1 = getPoint(bezier, r - 1, i, t);
    Vec p2 = getPoint(bezier, r - 1, i + 1, t);

    return ((1 - t)*p1 + t*p2);
}
