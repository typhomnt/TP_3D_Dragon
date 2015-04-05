#include "dragon.h"
#include "glCheck.h"


#include <QKeyEvent>


#include <iostream>
#include <cstdio>
#ifndef __APPLE__
#include <GL/glut.h>
#else
#include <GLUT/glut.h>
#endif
#include <QGLViewer/qglviewer.h>
#define T_GLOBALE 1
#define T_TAIL 0.1

using namespace std ;

Dragon::Dragon()
{
        c = new Cylinder(2,0.5,60);
}

Dragon::~Dragon(){
    delete c;
}

void Dragon::init(Viewer &v) {
    //c->init(v);
    // load textures
    tex_body = loadTexture("/images/texture_drag1.png");

    tex_field = loadTexture("/images/field1.jpg");

    program.load("shaders/texture.vert", "shaders/texture.frag");
    // get the program id and use it to have access to uniforms
    GLint program_id = (GLint)program;
    GLCHECK(glUseProgram( program_id ) );
    // get uniform locations (see the fragment shader)
    GLCHECK(texture0 = glGetUniformLocation( program_id, "texture0"));
    // get vertex texture coordinate locations
    GLCHECK(texcoord0 = glGetAttribLocation( program_id, "texcoord0"));
    // tex0 on the sampler will use the texture unit #0
    GLCHECK(glUniform1i( texture0, 0 ) );
}

void Dragon::draw(){

    // load the texture shader program: use this program instead of the fixed pipeline
    GLCHECK(glUseProgram( (GLint)program ));

    glPushMatrix();

    //draw the body
    //drawBody();
    //glRotatef(M_PI/2,1.0,0.0,0.0);
    //drawTail();
    //c->draw();
    drawBasePlane(5.0);
    glPopMatrix();

    GLCHECK(glUseProgram( 0 ));

}

void Dragon::drawBasePlane(float size){

    // active texture unit is 0
    GLCHECK(glActiveTexture(GL_TEXTURE0));

    // bind the texture for texture unit 0
    GLCHECK(glBindTexture(GL_TEXTURE_2D, tex_field));

    // set the texture sampler id in shader to active texture unit number
    GLCHECK(glUniform1i(texture0, 0));

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    float height = size;
    float s = 5.0;
    glNormal3f(0.0, 1.0, 1.0);
    glPushMatrix();
    glBegin(GL_QUADS);
    glVertexAttrib2f(texcoord0, 0, 0);
    glVertex3f(0, 0, height);
    glVertexAttrib2f(texcoord0, 1, 0);
    glVertex3f(s, 0, height);
    glVertexAttrib2f(texcoord0, 1, 1);
    glVertex3f(s, s, height);
    glVertexAttrib2f(texcoord0, 0, 1);
    glVertex3f(0, s, height);
    glEnd();
    glPopMatrix();

}

GLuint Dragon::loadTexture(const char *filename, bool mipmap)
{
    // generates an OpenGL texture id, and store it
    GLuint id;
    GLCHECK(glGenTextures(1, &id));

    // load a texture file as a QImage
    QImage img = QGLWidget::convertToGLFormat(QImage(filename));

    // specify the texture(2D texture, rgba, single file)
    GLCHECK(glBindTexture(GL_TEXTURE_2D, id));

    if (mipmap) {
        GLCHECK(gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.width(), img.height(),
                    GL_RGBA, GL_UNSIGNED_BYTE, img.bits()));
    }
    else {
        GLCHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0,
                    GL_RGBA, GL_UNSIGNED_BYTE, img.bits()));
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    return id;
}

void Dragon::drawTail(){
    /*glEnable(GL_FOG);
    GLfloat fog[4] = {0.5,0.5,0.5,1};
    glFogi(GL_FOG_MODE, GL_EXP);
    glFogfv(GL_FOG_COLOR, fog);
    glFogf(GL_FOG_DENSITY, 0.35);*/
    // Corps
    glScalef(T_TAIL,T_TAIL ,T_TAIL);
    glScalef(0.5,0.5,1.0);
    //glutSolidSphere(4,100,100);
    glScalef(2.0,2.0,1.0);
    glPushMatrix();

    //Queue
    glTranslatef(0,0,4.5);
    glutSolidSphere(0.2,100,100);
    glTranslatef(0,0,0.5);
    c->draw();
    glTranslatef(0,0,2.5);
    glutSolidSphere(0.2, 100, 100);
    glTranslatef(-0.25,0,0.5);
    glRotatef(-30,0,1,0);
    c->draw();
    glTranslatef(0,0,2.5);
    glutSolidSphere(0.2, 100, 100);
    glTranslatef(0,0,0.5);
    c->draw();

    glPushMatrix();
    glTranslatef(0,-0.25,2.3) ;
    glRotatef(45,1.0,0.0,0.0);
    glScalef(0.5,0.5,1.0);
    c->draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,0.25,2.3);
    glRotatef(-45,1.0,0.0,0.0);
    glScalef(0.5,0.5,1.0);
    c->draw();
    glPopMatrix();

    glTranslatef(0,0,2);
    glutSolidSphere(0.4,100,100);
    //glPopMatrix();
    //glPopMatrix();

    // Tete
    /*glPushMatrix();
    glScalef(T_TAIL,T_TAIL ,T_TAIL);
    glTranslatef(0,0,-4.5);
    glutSolidSphere(0.1,100,100);
    glTranslatef(0,0,-0.5);
    c->draw();
    glTranslatef(0,0,-2.5);
    glutSolidSphere(0.2, 100, 100);
    glTranslatef(0.25,0,-0.5);
    glRotatef(-30,0,1,0);
    c->draw();
    glTranslatef(0,0,-2.5);
    glutSolidSphere(0.2, 100, 100);
    glTranslatef(0,0,-0.5);
    c->draw();*/
    /*glScalef(T_TAIL,T_TAIL ,T_TAIL);
    c->draw();
    glPushMatrix();

    glTranslatef(0 ,-0.8,1.9) ;
    glRotatef(45,1.0,0.0,0.0);
    glScalef(0.5,0.5,1.0);
    c->draw();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0,0.8,1.9);
    glRotatef(-45,1.0,0.0,0.0);
    glScalef(0.5,0.5,1.0);
    c->draw();
    glPopMatrix();
    glTranslatef(0,0,2);
    glutSolidSphere(0.4,100,100);*/
}
