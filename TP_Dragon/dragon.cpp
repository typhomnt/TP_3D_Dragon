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


using namespace std;



Dragon::Dragon() {
    c = new Cylinder(2,0.5,60);
    t = new TrapezeIsocele(7.0,5.0,5.0,0.2);
    // Trapeze au bout des ailes
    t2 = new TrapezeIsocele(25.0/7.0,0.1,5.0,0.2);
}

Dragon::~Dragon() {
    delete c;
    delete t;
    delete t2;
}

void Dragon::init(Viewer &v) {
    c->init(v);
    // load textures
    tex_body = loadTexture("images/texture_drag1.png");
    tex_field = loadTexture("images/field1.jpg");

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
    drawTail();
    //c->draw();
    //drawBasePlane(5.0);
    glPopMatrix();

    GLCHECK(glUseProgram( 0 ));

}

void Dragon::drawBasePlane(float size) {
    GLCHECK(glActiveTexture(GL_TEXTURE0));
    GLCHECK(glBindTexture(GL_TEXTURE_2D, tex_field));
    GLCHECK(glUniform1i(texture0, 0));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glNormal3f(0.0, 0.0, 1.0);
    float height = 0;
    float s = size;

    glPushMatrix();
    glTranslatef(-s / 2.0, -s / 2.0, 0.0);

    // TODO... improve code for arbitrary scaling and texture tiling
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

    GLCHECK(glActiveTexture(GL_TEXTURE0));
    GLCHECK(glBindTexture(GL_TEXTURE_2D, tex_body));
    GLCHECK(glUniform1i(texture0, 0));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    c->updateTexture(texture0, texcoord0);

    // Corps
    glScalef(T_TAIL,T_TAIL ,T_TAIL);
    glScalef(4.0,4.0,4.0);
    c->draw();
    glScalef(0.25,0.25,0.25);
    glPushMatrix();

    //Queue
    glTranslatef(0,0,8.5);
    glutSolidSphere(0.2,100,100);
    glTranslatef(0,0,0.5);
    c->draw();
    glTranslatef(0,0,2.5);
    glutSolidSphere(0.2, 100, 100);
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
    glPopMatrix();
    //glPopMatrix();

    // Tete
    glPushMatrix();
    glTranslatef(0,0,-0.5);
    glutSolidSphere(0.2,100,100);
    glTranslatef(0,0,-2.5);
    c->draw();
    glTranslatef(0,0,-0.5);
    glutSolidSphere(0.2, 100, 100);
    glTranslatef(1.25,0,-2.5);
    glRotatef(-30,0,1,0);
    c->draw();
    glTranslatef(0,0,-0.5);
    glutSolidSphere(0.2, 100, 100);
    glTranslatef(0,0,-2.5);
    c->draw();
    glTranslatef(0,0,-0.5);
    glutSolidSphere(0.2, 100, 100);
    glTranslatef(0,0,-2.5);
    c->draw();
    glTranslatef(0,0,-2.5);
    glutSolidSphere(3,100,100);

    glPushMatrix();
    glTranslatef(2.5,-2.5,0);
    glRotatef(100.0,0.0,1.0,0.0);
    glRotatef(45.0,1.0,0.0,0.0);
    glutSolidCone(0.5,1.5,100,100);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2.5,2.5,0);
    glRotatef(100.0,0.0,1.0,0.0);
    glRotatef(-45.0,1.0,0.0,0.0);
    glutSolidCone(0.5,1.5,100,100);
    glPopMatrix();

    glPopMatrix();

    // Patte avant gauche

    glPushMatrix();
    glRotatef(-70.0,1.0,0.0,0.0);
    glRotatef(-60.0,0.0,1.0,0.0);
    glTranslatef(0.0,0.0,2.5);
    glutSolidSphere(0.2,100,100);
    glTranslatef(0.0,0.0,0.5);
    glRotatef(-20.0,1.0,0.0,0.0);
    c->draw();
    glTranslatef(0.0,0.0,2.5);
    glutSolidSphere(0.2,100,100);
    glTranslatef(0.0,0.0,0.5);
    c->draw();
    glTranslatef(0.0,0.0,2.5);
    glutSolidSphere(0.2,100,100);
    glTranslatef(0.0,0.0,0.5);
    c->draw();
    glTranslatef(0.0,0.0,3.5);
    glutSolidSphere(1.0,100,100);

    glPushMatrix();
    glRotatef(-90.0,1.0,0.0,0.0);
    glRotatef(-30.0,0.0,1.0,0.0);
    glTranslatef(0.0,0.0,1.2);
    glutSolidCone(0.2,1.5,100,100);
    glPopMatrix();

    glPushMatrix();
    glRotatef(-90.0,1.0,0.0,0.0);
    glRotatef(30.0,0.0,1.0,0.0);
    glTranslatef(0.0,0.0,1.2);
    glutSolidCone(0.2,1.5,100,100);
    glPopMatrix();

    glPopMatrix();
    // Patte avant droite

    glPushMatrix();
    glRotatef(70.0,1.0,0.0,0.0);
    glRotatef(-60.0,0.0,1.0,0.0);
    glTranslatef(0.0,0.0,2.5);
    glutSolidSphere(0.2,100,100);
    glTranslatef(0.0,0.0,0.5);
    glRotatef(20.0,1.0,0.0,0.0);
    c->draw();
    glTranslatef(0.0,0.0,2.5);
    glutSolidSphere(0.2,100,100);
    glTranslatef(0.0,0.0,0.5);
    c->draw();
    glTranslatef(0.0,0.0,2.5);
    glutSolidSphere(0.2,100,100);
    glTranslatef(0.0,0.0,0.5);
    c->draw();
    glTranslatef(0.0,0.0,3.5);
    glutSolidSphere(1.0,100,100);

    glPushMatrix();
    glRotatef(90.0,1.0,0.0,0.0);
    glRotatef(30.0,0.0,1.0,0.0);
    glTranslatef(0.0,0.0,1.2);
    glutSolidCone(0.2,1.5,100,100);
    glPopMatrix();

    glPushMatrix();
    glRotatef(90.0,1.0,0.0,0.0);
    glRotatef(-30.0,0.0,1.0,0.0);
    glTranslatef(0.0,0.0,1.2);
    glutSolidCone(0.2,1.5,100,100);
    glPopMatrix();

    glPopMatrix();

    // Patte arrière gauche

    glPushMatrix();
    glTranslatef(0.0,0.0,7.0);
    glRotatef(-70.0,1.0,0.0,0.0);
    glRotatef(-60.0,0.0,1.0,0.0);
    glTranslatef(0.0,0.0,2.5);
    glutSolidSphere(0.2,100,100);
    glTranslatef(0.0,0.0,0.5);
    glRotatef(-20.0,1.0,0.0,0.0);
    c->draw();
    glTranslatef(0.0,0.0,2.5);
    glutSolidSphere(0.2,100,100);
    glTranslatef(0.0,0.0,0.5);
    c->draw();
    glTranslatef(0.0,0.0,2.5);
    glutSolidSphere(0.2,100,100);
    glTranslatef(0.0,0.0,0.5);
    c->draw();
    glTranslatef(0.0,0.0,3.5);
    glutSolidSphere(1.0,100,100);

    glPushMatrix();
    glRotatef(-90.0,1.0,0.0,0.0);
    glRotatef(-30.0,0.0,1.0,0.0);
    glTranslatef(0.0,0.0,1.2);
    glutSolidCone(0.2,1.5,100,100);
    glPopMatrix();

    glPushMatrix();
    glRotatef(-90.0,1.0,0.0,0.0);
    glRotatef(30.0,0.0,1.0,0.0);
    glTranslatef(0.0,0.0,1.2);
    glutSolidCone(0.2,1.5,100,100);
    glPopMatrix();

    glPopMatrix();

    // Patte avant droite

    glPushMatrix();
    glTranslatef(0.0,0.0,7.0);
    glRotatef(70.0,1.0,0.0,0.0);
    glRotatef(-60.0,0.0,1.0,0.0);
    glTranslatef(0.0,0.0,2.5);
    glutSolidSphere(0.2,100,100);
    glTranslatef(0.0,0.0,0.5);
    glRotatef(20.0,1.0,0.0,0.0);
    c->draw();
    glTranslatef(0.0,0.0,2.5);
    glutSolidSphere(0.2,100,100);
    glTranslatef(0.0,0.0,0.5);
    c->draw();
    glTranslatef(0.0,0.0,2.5);
    glutSolidSphere(0.2,100,100);
    glTranslatef(0.0,0.0,0.5);
    c->draw();
    glTranslatef(0.0,0.0,3.5);
    glutSolidSphere(1.0,100,100);

    glPushMatrix();
    glRotatef(90.0,1.0,0.0,0.0);
    glRotatef(30.0,0.0,1.0,0.0);
    glTranslatef(0.0,0.0,1.2);
    glutSolidCone(0.2,1.5,100,100);
    glPopMatrix();

    glPushMatrix();
    glRotatef(90.0,1.0,0.0,0.0);
    glRotatef(-30.0,0.0,1.0,0.0);
    glTranslatef(0.0,0.0,1.2);
    glutSolidCone(0.2,1.5,100,100);
    glPopMatrix();

    glPopMatrix();

    // Aile gauche

    glPushMatrix();
    glTranslatef(0.0,2.5,0.5);
    glRotatef(-30.0,0.0,0.0,1.0);
    glScalef(0.4,0.4,3.5);
    c->draw();
    glScalef(2.5,2.5,1.0/3.5);
    glTranslatef(0.0,0.5,0.0);
    t->draw();
    glTranslatef(0.0,5.5,1.0);
    glScalef(0.4,0.4,2.5);
    c->draw();
    glScalef(2.5,2.5,0.4);
    glTranslatef(0.0,0.5,0.0);
    glRotatef(20.0,0.0,0.0,1.0);
    glScalef(1.0,1.0,5.0/7.0);
    t->draw();
    glScalef(1.0,1.0,7.0/5.0);
    glTranslatef(0.0,5.5,5.0/7.0);
    glScalef(0.4,0.4,25.0/14.0);
    c->draw();
    glScalef(2.5,2.5,14.0/25.0);
    glTranslatef(0.0,0.5,0.0);
    t2->draw();
    glPopMatrix();

    // Aile droite

    glPushMatrix();
    glTranslatef(0.0,-2.5,0.5);
    glRotatef(210.0,0.0,0.0,1.0);
    glScalef(0.4,0.4,3.5);
    c->draw();
    glScalef(2.5,2.5,1.0/3.5);
    glTranslatef(0.0,0.5,0.0);
    t->draw();
    glTranslatef(0.0,5.5,1.0);
    glScalef(0.4,0.4,2.5);
    c->draw();
    glScalef(2.5,2.5,0.4);
    glTranslatef(0.0,0.5,0.0);
    glRotatef(-20.0,0.0,0.0,1.0);
    glScalef(1.0,1.0,5.0/7.0);
    t->draw();
    glScalef(1.0,1.0,7.0/5.0);
    glTranslatef(0.0,5.5,5.0/7.0);
    glScalef(0.4,0.4,25.0/14.0);
    c->draw();
    glScalef(2.5,2.5,14.0/25.0);
    glTranslatef(0.0,0.5,0.0);
    t2->draw();
    glPopMatrix();
}



