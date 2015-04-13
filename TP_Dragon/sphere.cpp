#include "sphere.h"
#include "glCheck.h"
#include "vec.h"
#include "light.h"
#include "material.h"
#include <stdexcept>

#include <iostream>
#include <cstdio>
#ifndef __APPLE__
	#include <GL/glew.h>
	#include <GL/glut.h>
#else
	#include <GLUT/glut.h>
#endif
#include <QGLViewer/qglviewer.h>



static const vec4 black(0.0f, 0.0f, 0.0f, 1.0f);
static const vec4 white(1.0f, 1.0f, 1.0f, 1.0f);

static vec4 mat_ambient(0.7f, 0.7f, 0.7f, 1.0f);
static vec4 mat_ambient_color(0.8f, 0.8f, 0.2f, 1.0f);
static vec4 mat_diffuse(0.1f, 0.5f, 0.8f, 1.0f);

static const vec4 la(0.5f, 0.5f, 0.5f, 1.0f);
static const vec4 ld(0.5f, 0.5f, 0.5f, 1.0f);
static const vec4 ls(0.5f, 0.5f, 0.5f, 1.0f);
static Light light(vec4(0, 100, 100, 1), la, ld, ls);

static const Material material(mat_ambient_color, mat_diffuse, white, 5.0);



///////////////////////////////////////////////////////////////////////////////
Sphere::Sphere() : toggleCollisions(true) {}

///////////////////////////////////////////////////////////////////////////////
Sphere::Sphere(float x, float y, float z, float radius,float mass,GLint tex) : toggleCollisions(true) {
    this->x = x;
    this->y = y;
    this->z = z;
    if (radius < 0)
        std::invalid_argument("Radius must not be negative");
    this->radius = radius;
    if (mass < 0)
        std::invalid_argument("Mass must not be negative");
    this->mass = mass;
    this->invMass = (mass > 0 ? 1 / mass : 0.0);
    this->position = qglviewer::Vec(x,y,z);
    this->velocity = qglviewer::Vec(0,0,0);
    this->tex = tex;
}

Sphere::Sphere(qglviewer::Vec pos, qglviewer::Vec vel, float radius, float mass,GLint tex) : toggleCollisions(true) {
    this->x = pos[0];
    this->y = pos[1];
    this->z = pos[2];
    if (radius < 0)
        std::invalid_argument("Radius must not be negative");
    this->radius = radius;
    if (mass < 0)
        std::invalid_argument("Mass must not be negative");
    this->mass = mass;
    this->position = pos;
    this->velocity = vel;
    this->invMass = (mass > 0 ? 1 / mass : 0.0);
    this->tex = tex;
}

///////////////////////////////////////////////////////////////////////////////
void Sphere::init(Viewer &v) {
	(void)v; // Pour éviter un warning

	program.load("shaders/sphere.vert", "shaders/sphere.frag");
	GLint id = (GLint)program;
	
	GLCHECK(glUseProgram(id));
	GLCHECK(texture0 = glGetUniformLocation(id, "texture0"));
	GLCHECK(glUniform1i(texture0, 0));
	initLighting();
	GLCHECK(glUseProgram(0));
}

///////////////////////////////////////////////////////////////////////////////
void Sphere::initLighting() {
    GLCHECK(glUniform4fv(glGetUniformLocation(program, "light.la"), 1, &light.la.x));
    GLCHECK(glUniform4fv(glGetUniformLocation(program, "light.ld"), 1, &light.ld.x));
    GLCHECK(glUniform4fv(glGetUniformLocation(program, "light.ls" ), 1, &light.ls.x));
    GLCHECK(glUniform4fv(glGetUniformLocation(program, "light.position"), 1, &light.position.x));

    glEnable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
}


///////////////////////////////////////////////////////////////////////////////
void Sphere::draw() {
	GLCHECK(glUseProgram(program));

	GLCHECK(glActiveTexture(GL_TEXTURE0));
	GLCHECK(glBindTexture(GL_TEXTURE_2D, tex));
	GLCHECK(glUniform1i(texture0, 0));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	GLCHECK(glUniform4fv(glGetUniformLocation(program, "material.ka"), 1, &material.ka.x));
    GLCHECK(glUniform4fv(glGetUniformLocation(program, "material.kd"), 1, &material.kd.x));
    GLCHECK(glUniform4fv(glGetUniformLocation(program, "material.ks"), 1, &material.ks.x));
    GLCHECK(glUniform1f(glGetUniformLocation(program, "material.shininess"), material.shininess));

    glPushMatrix();
    glTranslatef(this->x, this->y, this->z);
	glutSolidSphere(this->radius, 100, 100);
    glPopMatrix();

	GLCHECK(glUseProgram(0));
}

///////////////////////////////////////////////////////////////////////////////
void Sphere::generate(float radius) {
    this->setRadius(radius);
    this->draw();
}

///////////////////////////////////////////////////////////////////////////////
float Sphere::getRadius() {
    return this->radius;
}

///////////////////////////////////////////////////////////////////////////////
float Sphere::getX() {
    return this->x;
}

///////////////////////////////////////////////////////////////////////////////
float Sphere::getY() {
    return this->y;
}

///////////////////////////////////////////////////////////////////////////////
float Sphere::getZ() {
    return this->z;
}

///////////////////////////////////////////////////////////////////////////////
void Sphere::setRadius(float radius) {
    if (radius < 0)
        std::invalid_argument("Radius must not be negative");
	this->radius = radius;
}

///////////////////////////////////////////////////////////////////////////////
void Sphere::setX(float x) {
    this->x = x;
}

///////////////////////////////////////////////////////////////////////////////
void Sphere::setY(float y) {
    this->y = y;
}

///////////////////////////////////////////////////////////////////////////////
void Sphere::setZ(float z) {
    this->z = z;
}

///////////////////////////////////////////////////////////////////////////////
void Sphere::operator=(const Sphere& s) {
    this->x = s.x;
    this->y = s.y;
    this->z = s.z;
    this->radius = s.radius;
}

///////////////////////////////////////////////////////////////////////////////
void Sphere::setTexture(GLint id) {
	this->tex = id;
}


///////////////////////////////////////////////////////////////////////////////
bool Sphere::getCollisions() {
	return toggleCollisions;
}


///////////////////////////////////////////////////////////////////////////////
void Sphere::setCollisions(bool enable) {
	this->toggleCollisions = enable;
}
///////////////////////////////////////////////////////////////////////////////

const qglviewer::Vec & Sphere::getPosition() const
{
    return position;
}

///////////////////////////////////////////////////////////////////////////////
const qglviewer::Vec & Sphere::getVelocity() const
{
    return velocity;
}

///////////////////////////////////////////////////////////////////////////////
float Sphere::getMass() const
{
    return mass;
}

///////////////////////////////////////////////////////////////////////////////
float Sphere::getInvMass() const
{
    return invMass;
}


///////////////////////////////////////////////////////////////////////////////
void Sphere::setPosition(const qglviewer::Vec &pos)
{
    this->position = pos;
    this->x = pos[0];
    this->y = pos[1];
    this->z = pos[2];
}

///////////////////////////////////////////////////////////////////////////////
void Sphere::setVelocity(const qglviewer::Vec &vel)
{
    this->velocity = vel;
}

///////////////////////////////////////////////////////////////////////////////
void Sphere::incrPosition(const qglviewer::Vec &pos)
{
    this->position += pos;
    this->x += pos[0];
    this->y += pos[1];
    this->z += pos[2];
}

///////////////////////////////////////////////////////////////////////////////
void Sphere::incrVelocity(const qglviewer::Vec &vel)
{
    this->velocity += vel;
}

