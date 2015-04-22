#include <iostream>
#ifndef __APPLE__
    #include <GL/glew.h>
    #include <GL/glut.h>
#else
    #include <GLUT/glut.h>
#endif
#include <QGLViewer/qglviewer.h>

#include "skybox.h"
#include "glCheck.h"


///////////////////////////////////////////////////////////////////////////////
Skybox::Skybox(float size, GLint texture0, GLint texcoord0) {
    this->size = size;
    this->texture0 = texture0;
    this->texcoord0 = texcoord0;
}


///////////////////////////////////////////////////////////////////////////////
void Skybox::init(Viewer &v) {
    (void)v;

    this->skyTex[0] = loadTexture("images/grass.jpg");
    this->skyTex[1] = loadTexture("images/skybox_top.jpg");
    this->skyTex[2] = loadTexture("images/skybox_front.jpg");
    this->skyTex[3] = loadTexture("images/skybox_back.jpg");
    this->skyTex[4] = loadTexture("images/skybox_left.jpg");
    this->skyTex[5] = loadTexture("images/skybox_right.jpg");

    GLCHECK(glUseProgram(program));
    GLCHECK(texture0 = glGetUniformLocation(program, "texture0"));
    GLCHECK(texcoord0 = glGetAttribLocation(program, "texcoord0"));
    GLCHECK(glUniform1i( texture0, 0 ) );
    GLCHECK(glUseProgram(0));
}


///////////////////////////////////////////////////////////////////////////////
void Skybox::draw() {

    GLCHECK(glUseProgram(program));

    GLCHECK(glActiveTexture(GL_TEXTURE0));
    GLCHECK(glBindTexture(GL_TEXTURE_2D, skyTex[0]));
    GLCHECK(glUniform1i(texture0, 0));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Face haute
    GLCHECK(glBindTexture(GL_TEXTURE_2D, skyTex[1]));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBegin(GL_QUADS);
        glNormal3f(0.0, 0.0, -1.0);
        glVertexAttrib2f(texcoord0, 0, 0);
        glVertex3f(-size/2, -size/2, size);
        glVertexAttrib2f(texcoord0, 0, 1);
        glVertex3f(size/2, -size/2, size);
        glVertexAttrib2f(texcoord0, 1, 1);
        glVertex3f(size/2, size/2, size);
        glVertexAttrib2f(texcoord0, 1, 0);
        glVertex3f(-size/2, size/2, size);
    glEnd();



    // Face latérale X positif
    GLCHECK(glBindTexture(GL_TEXTURE_2D, skyTex[2]));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBegin(GL_QUADS);
        glNormal3f(1.0, 0.0, 0.0);
        glVertexAttrib2f(texcoord0, 0, 0);
        glVertex3f(-size/2, -size/2, 0);
        glVertexAttrib2f(texcoord0, 1, 0);
        glVertex3f(-size/2, size/2, 0);
        glVertexAttrib2f(texcoord0, 1, 1);
        glVertex3f(-size/2, size/2, size);
        glVertexAttrib2f(texcoord0, 0, 1);
        glVertex3f(-size/2, -size/2, size);
    glEnd();


    // Face latérale X négatif
    GLCHECK(glBindTexture(GL_TEXTURE_2D, skyTex[3]));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBegin(GL_QUADS);
        glNormal3f(-1.0, 0.0, 0.0);
        glVertexAttrib2f(texcoord0, 0, 0);
        glVertex3f(size/2, size/2, 0.0);
        glVertexAttrib2f(texcoord0, 1, 0);
        glVertex3f(size/2, -size/2, 0.0);
        glVertexAttrib2f(texcoord0, 1, 1);
        glVertex3f(size/2, -size/2, size);
        glVertexAttrib2f(texcoord0, 0, 1);
        glVertex3f(size/2, size/2, size);
    glEnd();


    // Face latérale Y positif
    GLCHECK(glBindTexture(GL_TEXTURE_2D, skyTex[4]));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBegin(GL_QUADS);
        glNormal3f(0.0, 1.0, 0.0);
        glVertexAttrib2f(texcoord0, 0, 0);
        glVertex3f(size/2, -size/2, 0.0);
        glVertexAttrib2f(texcoord0, 1, 0);
        glVertex3f(-size/2, -size/2, 0.0);
        glVertexAttrib2f(texcoord0, 1, 1);
        glVertex3f(-size/2, -size/2, size);
        glVertexAttrib2f(texcoord0, 0, 1);
        glVertex3f(size/2, -size/2, size);
    glEnd();


    // Face latérale Y négatif
    GLCHECK(glBindTexture(GL_TEXTURE_2D, skyTex[5]));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBegin(GL_QUADS);
        glNormal3f(0.0, -1.0, 0.0);
        glVertexAttrib2f(texcoord0, 0, 0);
        glVertex3f(-size/2, size/2, 0.0);
        glVertexAttrib2f(texcoord0, 1, 0);
        glVertex3f(size/2, size/2, 0.0);
        glVertexAttrib2f(texcoord0, 1, 1);
        glVertex3f(size/2, size/2, size);
        glVertexAttrib2f(texcoord0, 0, 1);
        glVertex3f(-size/2, size/2, size);
    glEnd();

    GLCHECK(glUseProgram(0));
}



///////////////////////////////////////////////////////////////////////////////
void Skybox::setProgram(GLint program) {
    this->program = program;
}



///////////////////////////////////////////////////////////////////////////////
GLuint Skybox::loadTexture(const char *filename, bool mipmap) {
    GLuint id;
    GLCHECK(glGenTextures(1, &id));

    QImage img = QGLWidget::convertToGLFormat(QImage(filename));

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
