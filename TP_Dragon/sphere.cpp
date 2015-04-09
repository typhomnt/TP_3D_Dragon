#include "sphere.h"
#include "glCheck.h"
#include "vec.h"
#include "light.h"
#include "material.h"

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

	glutSolidSphere(this->radius, 100, 100);

	GLCHECK(glUseProgram(0));
}


///////////////////////////////////////////////////////////////////////////////
void Sphere::generate(float radius) {
	this->radius = radius;
	draw();
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