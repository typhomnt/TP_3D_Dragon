#ifndef _SPRING_H_
#define _SPRING_H_

#include <QGLViewer/vec.h>

#include "sphere.h"


class Spring
{
private:
    const Sphere *s1, *s2;

    double stiffness;
    double equilibriumLength;
    double damping;

public:
    /**
     *  Build a new Spring.
     *  @param[in] s1 the first particle of this spring
     *  @param[in] s2 the second particle of this spring
     *  @param[in] s spring stiffness
     *  @param[in] l equilibrium length
     *  @param[in] d damping factor
     */
    Spring(const Sphere *s1, const Sphere *s2,
           double s, double l0, double d);

        /**
     * Returns the force applied by this spring on particle 1 by
     * particle 2, out of their current positions and velocities.
     * The force applied on particle 2 is the opposite one.
     */
    qglviewer::Vec getCurrentForce() const;

    const Sphere *getParticle1() const;
    const Sphere *getParticle2() const;

    void draw() const;
};

#endif // _SPRING_H_
