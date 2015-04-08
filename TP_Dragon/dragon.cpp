#include "dragon.h"
#include "glCheck.h"
#include "vec.h"
#include "light.h"
#include "material.h"

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

// Data used for lighting
static const vec4 black(0.0f, 0.0f, 0.0f, 1.0f);
static const vec4 white(1.0f, 1.0f, 1.0f, 1.0f);
static const vec4 red(1.0f, 0.0f, 0.0f, 1.0f);
static vec4 no_color = black;

static vec4 mat_ambient(0.7f, 0.7f, 0.7f, 1.0f);
static vec4 mat_ambient_color(0.8f, 0.8f, 0.2f, 1.0f);
static vec4 mat_diffuse(0.1f, 0.5f, 0.8f, 1.0f);
static float no_shininess(0.0f);
static float low_shininess(5.0f);
static float high_shininess(50.0f);

static Light light(vec4(0, 100, 100, 1), white, white, white);

static const Material material(mat_ambient_color, mat_diffuse, white, low_shininess);




///////////////////////////////////////////////////////////////////////////////
Dragon::Dragon() {
    c = new Cylinder(2,0.5,60);
    t = new TrapezeIsocele(7.0,5.0,5.0,0.2);
    // Trapeze au bout des ailes
    t2 = new TrapezeIsocele(25.0/7.0,0.1,5.0,0.2);
    first_angle_wing = 0;
    first_angle_wing_up = true;
    second_angle_wing = 0;
    second_angle_wing_up = true;
    third_angle_wing = 0;
    third_angle_wing_up = true;
    time_wing1 = 0;
    time_wing2 = 0;
    time_wing3 = 0;
}


///////////////////////////////////////////////////////////////////////////////
Dragon::~Dragon() {
    delete c;
    delete t;
    delete t2;
}


///////////////////////////////////////////////////////////////////////////////
void Dragon::init(Viewer &v) {
    c->init(v);
    // load textures
    tex_body = loadTexture("images/texture_drag1.png");
    tex_field = loadTexture("images/field1.jpg");

    program.load("shaders/shader.vert", "shaders/shader.frag");
    // get the program id and use it to have access to uniforms
    GLint program_id = (GLint)program;
    GLCHECK(glUseProgram( program_id ) );
    // get uniform locations (see the fragment shader)
    GLCHECK(texture0 = glGetUniformLocation( program_id, "texture0"));
    // get vertex texture coordinate locations
    GLCHECK(texcoord0 = glGetAttribLocation( program_id, "texcoord0"));
    // tex0 on the sampler will use the texture unit #0
    GLCHECK(glUniform1i( texture0, 0 ) );

    initLighting();
}


///////////////////////////////////////////////////////////////////////////////
void Dragon::initLighting() {
    GLCHECK(glUniform4fv(glGetUniformLocation(program, "light.la"), 1, &light.la.x));
    GLCHECK(glUniform4fv(glGetUniformLocation(program, "light.ld"), 1, &light.ld.x));
    GLCHECK(glUniform4fv(glGetUniformLocation(program, "light.ls" ), 1, &light.ls.x));
    GLCHECK(glUniform4fv(glGetUniformLocation(program, "light.position"), 1, &light.position.x));

    glEnable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glClearColor(119.0 / 255.0, 181.0 / 255.0, 254.0 / 255.0, 0.0f);
}


///////////////////////////////////////////////////////////////////////////////
void Dragon::drawBasePlane(float size) {
    GLCHECK(glActiveTexture(GL_TEXTURE0));
    GLCHECK(glBindTexture(GL_TEXTURE_2D, tex_field));
    GLCHECK(glUniform1i(texture0, 0));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    GLCHECK(glUniform4fv(glGetUniformLocation(program, "material.ka"), 1, &material.ka.x));
    GLCHECK(glUniform4fv(glGetUniformLocation(program, "material.kd"), 1, &material.kd.x));
    GLCHECK(glUniform4fv(glGetUniformLocation(program, "material.ks"), 1, &material.ks.x));
    GLCHECK(glUniform1f(glGetUniformLocation(program, "material.shininess"), material.shininess));

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


///////////////////////////////////////////////////////////////////////////////
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

void Dragon::animate(){
    if(sin(time_wing1) > 0.9)
        first_angle_wing_up = false;
    if(sin(time_wing1) < -0.9)
        first_angle_wing_up = true;
    if(first_angle_wing_up)
    {
        first_angle_wing++;
        time_wing1+= 0.1;
    }
    else{
        first_angle_wing--;
        time_wing1-= 0.1;
    }

    if(sin(time_wing2 + M_PI/12) > 0.9)
        second_angle_wing_up = false;
    if(sin(time_wing2 + M_PI/12) < -0.9)
        second_angle_wing_up = true;
    if(second_angle_wing_up)
    {
        second_angle_wing++;
        time_wing2+= 0.1;
    }
    else{
        second_angle_wing--;
         time_wing2-= 0.1;
    }

    if(sin(time_wing3 + M_PI/6) > 0.9)
        third_angle_wing_up = false;
    if(sin(time_wing3 + M_PI/6) < -0.9)
        third_angle_wing_up = true;
    if(third_angle_wing_up)
    {
        third_angle_wing++;
        time_wing3+=0.1;
    }
    else{
        third_angle_wing--;
        time_wing3-=0.1;
    }
}

///////////////////////////////////////////////////////////////////////////////
void Dragon::draw(){

    /*glEnable(GL_FOG);
    GLfloat fog[4] = {0.5,0.5,0.5,1};
    glFogi(GL_FOG_MODE, GL_EXP);
    glFogfv(GL_FOG_COLOR, fog);
    glFogf(GL_FOG_DENSITY, 0.35);*/
    GLCHECK(glUseProgram( (GLint)program ));
    GLCHECK(glActiveTexture(GL_TEXTURE0));
    GLCHECK(glBindTexture(GL_TEXTURE_2D, tex_body));
    GLCHECK(glUniform1i(texture0, 0));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    c->updateTexture(texture0, texcoord0);
    glPushMatrix();

    drawBasePlane(5);

    glScalef(T_TAIL,T_TAIL ,T_TAIL);

    drawBody();

    glPushMatrix();
    drawTail();
    glPopMatrix();

    glPushMatrix();
    drawHead();
    glPopMatrix();

    glPushMatrix();
    drawPaw(true);
    glPopMatrix();

    glPushMatrix();
    drawPaw(false);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0,0.0,7.0);
    drawPaw(true);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0,0.0,7.0);
    drawPaw(false);
    glPopMatrix();

    glPushMatrix();
    drawWing(true);
    glPopMatrix();

    glPushMatrix();
    drawWing(false);
    glPopMatrix();
    
    glPopMatrix();
    

    GLCHECK(glUseProgram( 0 ));
}


///////////////////////////////////////////////////////////////////////////////
void Dragon::drawBody() {
    glScalef(4.0,4.0,4.0);
    c->draw();
    glScalef(0.25,0.25,0.25);
}


///////////////////////////////////////////////////////////////////////////////
void Dragon::drawTail() {
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
}


///////////////////////////////////////////////////////////////////////////////
void Dragon::drawHead() {
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
}


///////////////////////////////////////////////////////////////////////////////
void Dragon::drawClaw(bool leftPaw, bool leftClaw) {
    if (leftPaw)
        glRotatef(-90.0,1.0,0.0,0.0);
    else
        glRotatef(90.0,1.0,0.0,0.0);

    if (leftClaw)
        glRotatef(-30.0,0.0,1.0,0.0);
    else
        glRotatef(30.0,0.0,1.0,0.0);

    glTranslatef(0.0,0.0,1.2);
    glutSolidCone(0.2,1.5,100,100);
}


///////////////////////////////////////////////////////////////////////////////
void Dragon::drawPaw(bool left) {
    if (left)
        glRotatef(-70.0,1.0,0.0,0.0);
    else
        glRotatef(70.0,1.0,0.0,0.0);

    glRotatef(-60.0,0.0,1.0,0.0);
    glTranslatef(0.0,0.0,2.5);
    glutSolidSphere(0.2,100,100);

    glTranslatef(0.0,0.0,0.5);

    if (left)
        glRotatef(-20.0,1.0,0.0,0.0);
    else
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
    drawClaw(left, true);
    glPopMatrix();

    glPushMatrix();
    drawClaw(left, false);
    glPopMatrix();
}


///////////////////////////////////////////////////////////////////////////////
void Dragon::drawWing(bool left){
    if (left) {
        glTranslatef(0.0,2.5,0.5);
        glRotatef(-30.0,0.0,0.0,1.0);
    } else {
        glTranslatef(0.0,-2.5,0.5);
        glRotatef(210.0,0.0,0.0,1.0);
    }

    glScalef(0.4,0.4,3.5);
    c->draw();
    glScalef(2.5,2.5,1.0/3.5);
    glTranslatef(0.0,0.5,0.0);
    if(left)
        glRotatef(first_angle_wing,0.0,0.0,-1.0);
    else
        glRotatef(-first_angle_wing,0.0,0.0,-1.0);
    t->draw();
    glTranslatef(0.0,5.5,1.0);
    glScalef(0.4,0.4,2.5);
    c->draw();
    glScalef(2.5,2.5,0.4);
    glTranslatef(0.0,0.5,0.0);

    if (left)
        glRotatef(20.0,0.0,0.0,1.0);
    else
        glRotatef(-20.0,0.0,0.0,1.0);

    glScalef(1.0,1.0,5.0/7.0);
    if(left)
        glRotatef(second_angle_wing,0.0,0.0,-1.0);
    else
        glRotatef(-second_angle_wing,0.0,0.0,-1.0);
    t->draw();
    glScalef(1.0,1.0,7.0/5.0);
    glTranslatef(0.0,5.5,5.0/7.0);
    glScalef(0.4,0.4,25.0/14.0);
    c->draw();
    glScalef(2.5,2.5,14.0/25.0);
    glTranslatef(0.0,0.5,0.0);
    if(left)
        glRotatef(third_angle_wing,0.0,0.0,-1.0);
    else
        glRotatef(-third_angle_wing,0.0,0.0,-1.0);
    t2->draw();
}

