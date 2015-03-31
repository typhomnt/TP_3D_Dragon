#include <cmath>
#include <iostream>
#include <map>

#include "viewer.h"
#include "dynamicSystem.h"


DynamicSystem::DynamicSystem()
    :
    defaultGravity(0.0, 0.0, -10.0),
    defaultMediumViscosity(1.0),
    dt(0.1),
    groundPosition(0.0, 0.0, 0.0),
    groundNormal(0.0, 0.0, 1.0),
    rebound(0.5)
{
    // default values reset in init()
}

DynamicSystem::~DynamicSystem()
{
    clear();
}


void DynamicSystem::clear()
{
    std::vector<Particle *>::iterator itP;
    for (itP = particles.begin(); itP != particles.end(); ++itP) {
        delete(*itP);
    }
    particles.clear();

    std::vector<Spring *>::iterator itS;
    for (itS = springs.begin(); itS != springs.end(); ++itS) {
        delete(*itS);
    }
    springs.clear();
}

const qglviewer::Vec &DynamicSystem::getFixedParticlePosition() const
{
    return particles[0]->getPosition();	// no check on 0!
}

void DynamicSystem::setFixedParticlePosition(const qglviewer::Vec &pos)
{
    if (particles.size() > 0)
        particles[0]->setPosition(pos);
}

void DynamicSystem::setGravity(bool onOff)
{
    gravity = (onOff ? defaultGravity : qglviewer::Vec());
}

void DynamicSystem::setViscosity(bool onOff)
{
    mediumViscosity = (onOff ? defaultMediumViscosity : 0.0);
}

void DynamicSystem::setCollisionsDetection(bool onOff)
{
    handleCollisions = onOff;
}


///////////////////////////////////////////////////////////////////////////////
void DynamicSystem::init(Viewer &viewer)
{
    toggleGravity = true;
    toggleViscosity = true;
    toggleCollisions = true;
    clear();

    // global scene parameters
    defaultGravity = qglviewer::Vec(0.0, 0.0, -10.0);
    gravity = defaultGravity;
    defaultMediumViscosity = 1.0;
    mediumViscosity = defaultMediumViscosity;
    handleCollisions = true;
    dt = 0.01;
    groundPosition = qglviewer::Vec(0.0, 0.0, 0.0);
    groundNormal = qglviewer::Vec(0.0, 0.0, 1.0);
    rebound = 0.5;
    // parameters shared by all particles
    particleMass = 1.0;
    particleRadius = 0.25;
    distanceBetweenParticles = 4.0 * particleRadius;
    // parameters shared by all springs
    springStiffness = 30.0;
    springInitLength = 0.5;
    springDamping = 1.0;

    createSystemScene();
    // or another method, e.g. to test collisions on simple cases...
    //createTestCollisions();

    // add a manipulatedFrame to move particle 0 with the mouse
    viewer.setManipulatedFrame(new qglviewer::ManipulatedFrame());
    viewer.manipulatedFrame()->setPosition(getFixedParticlePosition());
    viewer.setSceneRadius(10.0f);

}


///////////////////////////////////////////////////////////////////////////////
void DynamicSystem::createSystemScene()
{
    // add a fixed particle
    qglviewer::Vec initPos = qglviewer::Vec(0.0, 0.0, 2.0);
    particles.push_back(new Particle(initPos, qglviewer::Vec(), 0, particleRadius));

    int nbParticles = 10; // Number of particles (with fixed particle)
    for (int i = 1; i < nbParticles; ++i) {
        Particle *fp = particles[i-1];
        qglviewer::Vec pos = fp->getPosition() + qglviewer::Vec(0.0, distanceBetweenParticles, 0.0);
        qglviewer::Vec vel = qglviewer::Vec(0.0, -20.0, 0.0);	// non null initial velocity

        Particle *np = new Particle(pos, vel, particleMass, particleRadius);
        particles.push_back(np);

        springs.push_back(new Spring(fp, np, 10.0, 2*distanceBetweenParticles, 1.0));
    }
}


///////////////////////////////////////////////////////////////////////////////
void DynamicSystem::createTestCollisions() {
    qglviewer::Vec pos = qglviewer::Vec(0.0, -2.0, 2.0);
    qglviewer::Vec vel = qglviewer::Vec(0.0, 0.0, 0.0);
    Particle *p1 = new Particle(pos, vel, 0, particleRadius);
    particles.push_back(p1);

    qglviewer::Vec pos1 = qglviewer::Vec(0.0, 2.0, 2.0);
    qglviewer::Vec vel1 = qglviewer::Vec(0.0, -1.0, 0.0);
    Particle *p2 = new Particle(pos1, vel1, particleMass, particleRadius);
    particles.push_back(p2);

    //springs.push_back(new Spring(p1, p2, 2, 5, 2));
}


///////////////////////////////////////////////////////////////////////////////
void DynamicSystem::draw()
{
    // Particles
    glColor3f(1,0,0);
    std::vector<Particle *>::iterator itP;
    for (itP = particles.begin(); itP != particles.end(); ++itP) {
        (*itP)->draw();
    }

    // Springs
    glColor3f(1.0, 0.28, 0.0);
    glLineWidth(5.0);
    std::vector<Spring *>::iterator itS;
    for (itS = springs.begin(); itS != springs.end(); ++itS) {
        (*itS)->draw();
    }

    // Ground :
    glColor3f(0.0, 0.0, 1.0);
    glBegin(GL_QUADS);
    glVertex3f(-10.0f, -10.0f, 0.0f);
    glVertex3f(-10.0f, 10.0f,  0.0f);
    glVertex3f( 10.0f, 10.0f,  0.0f);
    glVertex3f( 10.0f, -10.0f, 0.0f);
    glEnd();
}


///////////////////////////////////////////////////////////////////////////////
void DynamicSystem::animate()
{

//======== 1. Compute all forces
    // map to accumulate the forces to apply on each particle
    std::map<const Particle *, qglviewer::Vec> forces;

    // weights
    std::vector<Particle *>::iterator itP;
    for (itP = particles.begin(); itP != particles.end(); ++itP) {
        Particle *p = *itP;
        forces[p] = gravity * p->getMass();
    }

    // damped springs
    std::vector<Spring *>::iterator itS;
    for (itS = springs.begin(); itS != springs.end(); ++itS) {
        Spring *s = *itS;
        qglviewer::Vec f12 = s->getCurrentForce();
        forces[s->getParticle1()] += f12;
        forces[s->getParticle2()] -= f12; // opposite force
    }

    // medium viscosity
    for (itP = particles.begin(); itP != particles.end(); ++itP) {
        Particle *p = *itP;
        forces[p] -= mediumViscosity * p->getVelocity();
    }


//======== 2. Integration scheme
    // update particles velocity (qu. 1)
    for (itP = particles.begin(); itP != particles.end(); ++itP) {
        Particle *p = *itP;
        p->incrVelocity(dt*p->getInvMass()*forces[p]);
    }

    // update particles positions
    for (itP = particles.begin(); itP != particles.end(); ++itP) {
        Particle *p = *itP;
        // q = q + dt * v
        p->incrPosition(dt * p->getVelocity());
    }


//======== 3. Collisions
    if (handleCollisions) {
        //TO DO: discuss multi-collisions and order!
        for (itP = particles.begin(); itP != particles.end(); ++itP) {
            collisionParticleGround(*itP);
        }
        for(unsigned int i = 0; i < particles.size(); ++i) {
            for(unsigned int j = 0; j < i; ++j){
                    Particle *p1 = particles[i];
                    Particle *p2 = particles[j];
                    collisionParticleParticle(p1, p2);

            }
        }
    }
}



void DynamicSystem::collisionParticleGround(Particle *p)
{
    // don't process fixed particles (ground plane is fixed)
    if (p->getInvMass() == 0)
        return;

    // particle-plane distance
    double penetration = (p->getPosition() - groundPosition) * groundNormal;
    penetration -= p->getRadius();
    if (penetration >= 0)
        return;

    // penetration velocity
    double vPen = p->getVelocity() * groundNormal;

    // updates position and velocity of the particle
    p->incrPosition(-penetration * groundNormal);
    p->incrVelocity(-(1 + rebound) * vPen * groundNormal);
}

/*
void DynamicSystem::collisionParticleParticle(Particle *p1, Particle *p2)
{
    // don't process fixed particles
    if (p1->getMass() == 0 && p2->getMass() == 0)
        return;

    // particle-particle distance
    qglviewer::Vec n = p2->getPosition() - p1->getPosition();
    double penetration = sqrt(n*n);
    penetration -= (p1->getRadius() + p2->getRadius());
    if (penetration >= 0)
        return;

    // penetration velocity
    n.normalize();
    double m1 = p1->getMass() / (p1->getMass() + p2->getMass());
    double m2 = p2->getMass() / (p1->getMass() + p2->getMass());
    double vPen1 = p1->getVelocity() * n * m1;
    double vPen2 = p2->getVelocity() * n * m2;

    // updates position and velocity
    p1->incrPosition(-penetration * n * m1);
    p1->incrVelocity(-(1 + rebound) * vPen1 * n);
    p2->incrPosition(penetration * n * m2);
    p2->incrVelocity(-(1 + rebound) * vPen2 * n);

}
*/

void DynamicSystem::collisionParticleParticle(Particle *p1, Particle *p2)
{
    // don't process fixed particles
    if (p1->getMass() == 0 && p2->getMass() == 0)
        return;

    // particle-particle distance
    qglviewer::Vec n = p1->getPosition() - p2->getPosition();
    double penetration = sqrt(n*n);
    penetration -= (p1->getRadius() + p2->getRadius());
    if (penetration >= 0)
        return;

    std::cout << "collision" << std::endl;
    n.normalize();
    double m1 = p1->getMass(), m2 = p2->getMass();

    qglviewer::Vec v1 = p1->getVelocity();
    qglviewer::Vec v1x = (n * v1) * n;
    qglviewer::Vec v1y = v1 - v1x;

    qglviewer::Vec v2 = p2->getVelocity();
    n = -n;
    qglviewer::Vec v2x = (n * v2) * n;
    qglviewer::Vec v2y = v2 - v2x;

    // new velocities
    if (m1 != 0)
        p1->setVelocity(-v1x * (m1-m2)/(m1+m2)  + v2x * (2*m2/(m1+m2)) + v1y);
    if (m2 != 0)
        p2->setVelocity(v1x * (2*m1)/(m1+m2) + v2x * ((m2-m1)/(m1+m2)) + v2y);

    if (m1 == 0)
        p2->setVelocity(-(1 + rebound) * p2->getVelocity());
    if (m2 == 0)
        p1->setVelocity((1 + rebound) * p1->getVelocity());
}


void DynamicSystem::keyPressEvent(QKeyEvent* e, Viewer& viewer)
{
    // Get event modifiers key
    const Qt::KeyboardModifiers modifiers = e->modifiers();

        /* Controls added for Lab Session 6 "Physicall Modeling" */
        if ((e->key()==Qt::Key_G) && (modifiers==Qt::NoButton)) {
        toggleGravity = !toggleGravity;
        setGravity(toggleGravity);
        viewer.displayMessage("Set gravity to "
            + (toggleGravity ? QString("true") : QString("false")));

    } else if ((e->key()==Qt::Key_V) && (modifiers==Qt::NoButton)) {
        toggleViscosity = !toggleViscosity;
        setViscosity(toggleViscosity);
        viewer.displayMessage("Set viscosity to "
            + (toggleViscosity ? QString("true") : QString("false")));

    } else if ((e->key()==Qt::Key_C) && (modifiers==Qt::NoButton)) {
        toggleCollisions = !toggleCollisions;
        setCollisionsDetection(toggleCollisions);
        viewer.displayMessage("Detects collisions "
            + (toggleCollisions ? QString("true") : QString("false")));

    } else if ((e->key()==Qt::Key_Home) && (modifiers==Qt::NoButton)) {
        // stop the animation, and reinit the scene
        viewer.stopAnimation();
        init(viewer);
        viewer.manipulatedFrame()->setPosition(getFixedParticlePosition());
        toggleGravity = true;
        toggleViscosity = true;
        toggleCollisions = true;
    }
}

void DynamicSystem::mouseMoveEvent(QMouseEvent*, Viewer& v)
{
        setFixedParticlePosition(v.manipulatedFrame()->position());
}

