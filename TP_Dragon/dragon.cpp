#include "dragon.h"
#include "glCheck.h"
#include "vec.h"
#include "light.h"
#include "material.h"
#include <cmath>
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

static const vec4 la(0.5f, 0.5f, 0.5f, 1.0f);
static const vec4 ld(0.5f, 0.5f, 0.5f, 1.0f);
static const vec4 ls(0.5f, 0.5f, 0.5f, 1.0f);
static Light light(vec4(0, 100, 100, 1), la, ld, ls);

static const Material material(mat_ambient_color, mat_diffuse, white, low_shininess);

//Data used for dynamics

static qglviewer::Vec defaultGravity;
static qglviewer::Vec gravity;
static float viscosity;
static float dt;
static  qglviewer::Vec groundPosition = qglviewer::Vec(0.0, 0.0, -5.0);
static  qglviewer::Vec groundNormal = qglviewer::Vec(0.0, 0.0, 1.0);
static float rebound = 0.5;
static qglviewer::Vec fly_force = qglviewer::Vec(0,0,0);

/*nouveau dragon*/

///////////////////////////////////////////////////////////////////////////////
Dragon::Dragon() {
    t = new TrapezeIsocele(7.0,5.0,5.0,0.2);
    // Trapeze au bout des ailes
    t2 = new TrapezeIsocele(25.0/7.0,0.1,5.0,0.2);
    R = 0.5;
    nbSpheresBody = 15;
    nbSpheresTail = 25;
    nbSpheresNeck = 15;
    nbSpheresPaw = 13;
    indexBody = 0;
    indexTail = indexBody + nbSpheresBody;
    indexNeck = indexTail + nbSpheresTail;
    indexPawLeftUp = indexNeck + nbSpheresNeck;
    indexPawRightUp = indexPawLeftUp + nbSpheresPaw;
    indexPawLeftDown = indexPawRightUp + nbSpheresPaw;
    indexPawRightDown = indexPawLeftDown + nbSpheresPaw;
    indexLastPawRightDown = indexPawRightDown + nbSpheresPaw-1;
    thicknessBody = 5*R;
    mass = 3000;
    qglviewer::Vec initPos = qglviewer::Vec(0,0,15);
    qglviewer::Vec initVec = qglviewer::Vec(0,0,0);
    dragPart = new Sphere(initPos,initVec,R,mass);
    first_angle_wing = 0;
    first_angle_wing_up = true;
    second_angle_wing = 0;
    second_angle_wing_up = true;
    third_angle_wing = 0;
    third_angle_wing_up = true;
    time_wing1 = 0;
    time_wing2 = 0;
    time_wing3 = 0;
    fly_up = true;
    dist_flyx = 0.1;
    dist_flyy = 0.1;
    dist_flyz = 0.1;
    createBody(indexBody, indexTail-1);
    createTail(30.0, indexTail, indexNeck-1);
    createNeck(indexNeck, indexPawLeftUp-1);
    createPawLeftUp(-70, indexPawLeftUp, indexPawRightUp-1);
    createPawRightUp(-110, indexPawRightUp, indexPawLeftDown-1);
    createPawLeftDown(-70, indexPawLeftDown, indexPawRightDown-1);
    createPawRightDown(-110, indexPawRightDown, indexLastPawRightDown);
    //drawSkeleton();
    //this->skeleton.push_back(dragPart);
}


///////////////////////////////////////////////////////////////////////////////
Dragon::~Dragon() {
    delete t;
    delete t2;
    delete dragPart;
    for(std::vector<Sphere*>::iterator it = skeleton.begin() ; it != skeleton.end(); it++){
        Sphere* s = *it;
        delete s;
    }
    for(std::vector<Sphere*>::iterator it = body.begin() ; it != body.end(); it++){
        Sphere* s = *it;
        delete s;
    }
    for(std::vector<Sphere*>::iterator it = tail.begin() ; it != tail.end(); it++){
        Sphere* s = *it;
        delete s;
    }
    for(std::vector<Sphere*>::iterator it = neck.begin() ; it != neck.end(); it++){
        Sphere* s = *it;
        delete s;
    }
    for(std::vector<Sphere*>::iterator it = pawLeftUp.begin() ; it != pawLeftUp.end(); it++){
        Sphere* s = *it;
        delete s;
    }
    for(std::vector<Sphere*>::iterator it = pawRightUp.begin() ; it != pawRightUp.end(); it++){
        Sphere* s = *it;
        delete s;
    }
    for(std::vector<Sphere*>::iterator it = pawLeftDown.begin() ; it != pawLeftDown.end(); it++){
        Sphere* s = *it;
        delete s;
    }
    for(std::vector<Sphere*>::iterator it = pawRightDown.begin() ; it != pawRightDown.end(); it++){
        Sphere* s = *it;
        delete s;
    }
}


///////////////////////////////////////////////////////////////////////////////
void Dragon::init(Viewer &v) {
    // load textures
    tex_skeleton = loadTexture("images/texture_drag1.png");
    tex_field = loadTexture("images/field1.jpg");
    tex_body = loadTexture("images/peau_dragon.jpeg");

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

    defaultGravity = qglviewer::Vec(0.0, 0.0, -10.0);
    gravity = defaultGravity;
    viscosity = 1.0;
    dt = 0.01;
    for(std::vector<Sphere*>::iterator it = skeleton.begin() ; it != skeleton.end(); it++){
        Sphere* s = *it;
        s->setTexture(tex_skeleton);
        s->init(v);
    }
    for(std::vector<Sphere*>::iterator it = body.begin() ; it != body.end(); it++){
        Sphere* s = *it;
        s->setTexture(tex_body);
        s->init(v);
    }
    for(std::vector<Sphere*>::iterator it = tail.begin() ; it != tail.end(); it++){
        Sphere* s = *it;
        s->setTexture(tex_body);
        s->init(v);
    }
    for(std::vector<Sphere*>::iterator it = neck.begin() ; it != neck.end(); it++){
        Sphere* s = *it;
        s->setTexture(tex_body);
        s->init(v);
    }
    for(std::vector<Sphere*>::iterator it = pawLeftUp.begin() ; it != pawLeftUp.end(); it++){
        Sphere* s = *it;
        s->setTexture(tex_body);
        s->init(v);
    }
    for(std::vector<Sphere*>::iterator it = pawRightUp.begin() ; it != pawRightUp.end(); it++){
        Sphere* s = *it;
        s->setTexture(tex_body);
        s->init(v);
    }
    for(std::vector<Sphere*>::iterator it = pawLeftDown.begin() ; it != pawLeftDown.end(); it++){
        Sphere* s = *it;
        s->setTexture(tex_body);
        s->init(v);
    }
    for(std::vector<Sphere*>::iterator it = pawRightDown.begin() ; it != pawRightDown.end(); it++){
        Sphere* s = *it;
        s->setTexture(tex_body);
        s->init(v);
    }
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
    float s = size;

    glPushMatrix();
    glTranslatef(-s / 2.0, -s / 2.0, 0.0);

    // TODO... improve code for arbitrary scaling and texture tiling
    glBegin(GL_QUADS);  
    glVertexAttrib2f(texcoord0, 0, 0);
    glVertex3f(groundPosition[0],groundPosition[1],groundPosition[2]);
    glVertexAttrib2f(texcoord0, 1, 0);
    glVertex3f(s + groundPosition[0] , groundPosition[1], groundPosition[2]);
    glVertexAttrib2f(texcoord0, 1, 1);
    glVertex3f(s + groundPosition[0], s + groundPosition[1], groundPosition[2]);
    glVertexAttrib2f(texcoord0, 0, 1);
    glVertex3f(groundPosition[0], s + groundPosition[1], groundPosition[2]);
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

/////////////////////////////////////////////////////////////////////
void Dragon::animate(){
    if(sin(time_wing1) > 0.9)
        first_angle_wing_up = false;
    if(sin(time_wing1) < -0.9)
        first_angle_wing_up = true;
    if(first_angle_wing_up)
    {
        first_angle_wing++;
        fly_up = true;
        time_wing1+= 0.1;
    }
    else{
        first_angle_wing--;
        fly_up = false;
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

    std::map<const Sphere *, qglviewer::Vec> forces;
    for(std::vector<Sphere*>::iterator it = skeleton.begin() ; it != skeleton.end(); it++){
        Sphere* s = *it;
        forces[s] = gravity * s->getMass();
        forces[s] -= viscosity*s->getVelocity();
        forces[s] += fly_force ;
        s->incrVelocity(dt*s->getInvMass()*forces[s]);
        s->incrPosition(dt * s->getVelocity());
        collisionParticleGround(s);
    }
    fly_force[0] = 0;
    fly_force[1] = 0;
    fly_force[2] = 0;
}

///////////////////////////////////////////////////////////////////////////////
void Dragon::draw(){

    GLCHECK(glUseProgram( (GLint)program ));
    glPushMatrix();
    drawBasePlane(50.0);
    glPopMatrix();
    GLCHECK(glActiveTexture(GL_TEXTURE0));
    GLCHECK(glBindTexture(GL_TEXTURE_2D, tex_skeleton));
    GLCHECK(glUniform1i(texture0, 0));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glPushMatrix();
    drawBody(indexBody, indexTail-1);
    glPopMatrix();

    glPushMatrix();
    drawTail(indexTail, indexNeck-1);
    glPopMatrix();

    glPushMatrix();
    drawNeck(indexNeck, indexPawLeftUp-1);
    glPopMatrix();

    glPushMatrix();
    drawPawLeftUp(indexPawLeftUp, indexPawRightUp-1);
    glPopMatrix();

    glPushMatrix();
    drawPawRightUp(indexPawRightUp, indexPawLeftDown-1);
    glPopMatrix();

    glPushMatrix();
    drawPawLeftDown(indexPawLeftDown, indexPawRightDown-1);
    glPopMatrix();

    glPushMatrix();
    drawPawRightDown(indexPawRightDown, indexLastPawRightDown);
    glPopMatrix();

    glPopMatrix();
    
    GLCHECK(glUseProgram( 0 ));
}


///////////////////////////////////////////////////////////////////////////////
void Dragon::drawBody(int first, int last) {
    skeleton[first]->draw();
    for (int i = first+1; i <= last; i++) {
        skeleton[i]->draw();
    }
    for(std::vector<Sphere*>::iterator it = body.begin() ; it != body.end(); it++){
        Sphere* s = *it;
        s->draw();
    }
}


///////////////////////////////////////////////////////////////////////////////
void Dragon::drawTail(int first, int last) {
    for (int i = first; i <= last; i++) {
        skeleton[i]->draw();
    }
    for(std::vector<Sphere*>::iterator it = tail.begin() ; it != tail.end(); it++){
        Sphere* s = *it;
        s->draw();
    }
}


///////////////////////////////////////////////////////////////////////////////
void Dragon::drawNeck(int first, int last) {
    skeleton[first]->draw();
    for (int i = first+1; i <= last; i++) {
        skeleton[i]->draw();
    }
    for(std::vector<Sphere*>::iterator it = neck.begin() ; it != neck.end(); it++){
        Sphere* s = *it;
        s->draw();
    }
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
void Dragon::drawPawLeftUp(int first, int last) {
    skeleton[first]->draw();
    for (int i = first+1; i <= last; i++) {
        skeleton[i]->draw();
    }
    for(std::vector<Sphere*>::iterator it = pawLeftUp.begin() ; it != pawLeftUp.end(); it++){
        Sphere* s = *it;
        s->draw();
    }
}

///////////////////////////////////////////////////////////////////////////////
void Dragon::drawPawRightUp(int first, int last) {
    skeleton[first]->draw();
    for (int i = first+1; i <= last; i++) {
        skeleton[i]->draw();
    }
    for(std::vector<Sphere*>::iterator it = pawRightUp.begin() ; it != pawRightUp.end(); it++){
        Sphere* s = *it;
        s->draw();
    }

}

///////////////////////////////////////////////////////////////////////////////
void Dragon::drawPawLeftDown(int first, int last) {
    skeleton[first]->draw();
    for (int i = first+1; i <= last; i++) {
        skeleton[i]->draw();
    }
    for(std::vector<Sphere*>::iterator it = pawLeftDown.begin() ; it != pawLeftDown.end(); it++){
        Sphere* s = *it;
        s->draw();
    }

}

///////////////////////////////////////////////////////////////////////////////
void Dragon::drawPawRightDown(int first, int last) {
    skeleton[first]->draw();
    for (int i = first+1; i <= last; i++) {
        skeleton[i]->draw();
    }
    for(std::vector<Sphere*>::iterator it = pawRightDown.begin() ; it != pawRightDown.end(); it++){
        Sphere* s = *it;
        s->draw();
    }
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
    glScalef(2.5,2.5,1.0/3.5);
    glTranslatef(0.0,0.5,0.0);
    if(left)
        glRotatef(first_angle_wing,0.0,0.0,-1.0);
    else
        glRotatef(-first_angle_wing,0.0,0.0,-1.0);
    t->draw();
    glTranslatef(0.0,5.5,1.0);
    glScalef(0.4,0.4,2.5);
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
    glScalef(2.5,2.5,14.0/25.0);
    glTranslatef(0.0,0.5,0.0);
    if(left)
        glRotatef(third_angle_wing,0.0,0.0,-1.0);
    else
        glRotatef(-third_angle_wing,0.0,0.0,-1.0);
    t2->draw();
}

/////////////////////////////////////////////////////////////////////
void Dragon::createBody(int first, int last){
    float height = 10;
    skeleton.push_back(new Sphere(0,0,height,R,10,tex_skeleton));
    for (int i = first+1; i <= last; i++) {
        skeleton.push_back(new Sphere(skeleton[i-1]->getX()+2*R,0,skeleton[0]->getZ(),R,10,tex_skeleton));
    }
    for (int i = first; i <= last; i++) {
        for (int j = 0; j <= 23; j++) {
            body.push_back(new Sphere(skeleton[i]->getX(),
                                      skeleton[i]->getY() + thicknessBody*cos(M_PI/12*j),
                                      skeleton[i]->getZ() + thicknessBody*sin(M_PI/12*j),
                                      R,10,tex_body));
            body.push_back(new Sphere(skeleton[i]->getX() + R,
                                      skeleton[i]->getY() + thicknessBody*cos(M_PI/12*j),
                                      skeleton[i]->getZ() + thicknessBody*sin(M_PI/12*j),
                                      R,10,tex_body));
        }
    }
}

////////////////////////////////////////////////////////////////////
void Dragon::createTail(float angle, int first, int last){
    float x0 = skeleton[first-1]->getX();
    float z0 = skeleton[first-1]->getZ();
    for (int i = first; i <= last; i++) {
        float x = x0 + 2*R*(i-first+1);
        skeleton.push_back(new Sphere(x,
                                      0,
                                      z0 + (x-x0)*(x-x0)/40.0,
                                      R,10,tex_skeleton));
    }
    /*for (int i = (last+first)/2 + 1; i <= last; i++) {
        float diff = (last-(last+first)/2+2);
        float dx = - 2*R*cos(M_PI/diff*(last-i))*(1 + cos(M_PI/diff*(last-i)));
        float dz = 2*R*sin(M_PI/diff*(last-i))*(1 + cos(M_PI/diff*(last-i)));
        if (dx*dx + dz*dz > 4*R*R)
            skeleton.push_back(new Sphere(skeleton[i-1]->getX() + dx,
                                          skeleton[i-1]->getY(),
                                          skeleton[i-1]->getZ() + dz,
                                          R));
        else
            skeleton.push_back(new Sphere(skeleton[i-1]->getX(),
                                          skeleton[i-1]->getY(),
                                          skeleton[i-1]->getZ(),
                                          R));
    }*/

    float thicknessTail = 5*R;
    float x1,x2,z1,z2;
    for (int i = first; i <= (last+first)/2; i++) {
        x1 = skeleton[i]->getX() - skeleton[i-1]->getX();
        z1 = skeleton[i]->getZ() - skeleton[i-1]->getZ();
        x2 = skeleton[i+1]->getX() - skeleton[i]->getX();
        z2 = skeleton[i+1]->getZ() - skeleton[i]->getZ();
        for (int j = 0; j <= 23; j++) {
            tail.push_back(new Sphere(skeleton[i]->getX() + thicknessTail*cos(M_PI/12*j)*(-z1),
                                      skeleton[i]->getY() + thicknessTail*sin(M_PI/12*j),
                                      skeleton[i]->getZ() + thicknessTail*cos(M_PI/12*j)*x1,
                                      R,10,tex_body));
            tail.push_back(new Sphere(skeleton[i]->getX() + x2/2.0 + thicknessTail*cos(M_PI/12*j)*(-z1),
                                      skeleton[i]->getY() + thicknessTail*sin(M_PI/12*j),
                                      skeleton[i]->getZ() + z2/2.0 + thicknessTail*cos(M_PI/12*j)*x1,
                                      R,10,tex_body));

        }
        thicknessTail = thicknessTail - 4*R/((last-first)/2+1);
    }
    for (int i = (last+first)/2 + 1; i < last; i++) {
        x1 = skeleton[i]->getX() - skeleton[i-1]->getX();
        z1 = skeleton[i]->getZ() - skeleton[i-1]->getZ();
        x2 = skeleton[i+1]->getX() - skeleton[i]->getX();
        z2 = skeleton[i+1]->getZ() - skeleton[i]->getZ();
        for (int j = 0; j <= 23; j++) {
            tail.push_back(new Sphere(skeleton[i]->getX() + thicknessTail*cos(M_PI/12*j)*(-z1),
                                      skeleton[i]->getY() + thicknessTail*sin(M_PI/12*j),
                                      skeleton[i]->getZ() + thicknessTail*cos(M_PI/12*j)*x1,
                                      R,10,tex_body));
            tail.push_back(new Sphere(skeleton[i]->getX() + x2/2.0 + thicknessTail*cos(M_PI/12*j)*(-z1),
                                      skeleton[i]->getY() + thicknessTail*sin(M_PI/12*j),
                                      skeleton[i]->getZ() + z2/2.0 + thicknessTail*cos(M_PI/12*j)*x1,
                                      R,10,tex_body));
        }
        thicknessTail = thicknessTail-2*R/(last-first);
    }
    x1 = skeleton[last]->getX() - skeleton[last-1]->getX();
    z1 = skeleton[last]->getZ() - skeleton[last-1]->getZ();
    for (int j = 0; j <= 23; j++) {
        tail.push_back(new Sphere(skeleton[last]->getX() + thicknessTail*cos(M_PI/12*j)*(-z1),
                                  skeleton[last]->getY() + thicknessTail*sin(M_PI/12*j),
                                  skeleton[last]->getZ() + thicknessTail*cos(M_PI/12*j)*x1,
                                  R,10,tex_body));
    }
}


/////////////////////////////////////////////////////////////////////
void Dragon::createNeck(int first, int last){
    skeleton.push_back(new Sphere(skeleton[indexBody]->getX()-2*R,
                                  0,
                                  skeleton[indexBody]->getZ(),
                                  R,10,tex_skeleton));
    float x0 = skeleton[indexNeck]->getX();
    float z0 = skeleton[indexNeck]->getZ();
    for (int i = first+1; i <= last; i++) {
        float x = x0 - 2*R*(i-first);
        skeleton.push_back(new Sphere(x,
                                      0,
                                      z0 + (x-x0)*(x-x0)/15.0,
                                      R,10,tex_skeleton));
    }
    float thicknessNeck = 5*R;
    float x1,x2,z1,z2;
    for (int j = 0; j <= 23; j++) {
        neck.push_back(new Sphere(skeleton[first]->getX() - R,
                                  skeleton[first]->getY() + thicknessNeck*cos(M_PI/12*j),
                                  skeleton[first]->getZ() + thicknessNeck*sin(M_PI/12*j),
                                  R,10,tex_body));
        neck.push_back(new Sphere(skeleton[first]->getX(),
                                  skeleton[first]->getY() + thicknessNeck*cos(M_PI/12*j),
                                  skeleton[first]->getZ() + thicknessNeck*sin(M_PI/12*j),
                                  R,10,tex_body));
        neck.push_back(new Sphere(skeleton[first]->getX() + R,
                                  skeleton[first]->getY() + thicknessNeck*cos(M_PI/12*j),
                                  skeleton[first]->getZ() + thicknessNeck*sin(M_PI/12*j),
                                  R,10,tex_body));
    }
    thicknessNeck = thicknessNeck - 3*R/((last-first)/2+1);
    for (int i = first+1; i <= (last+first)/2; i++) {
        x1 = skeleton[i]->getX() - skeleton[i-1]->getX();
        z1 = skeleton[i]->getZ() - skeleton[i-1]->getZ();
        x2 = skeleton[i+1]->getX() - skeleton[i]->getX();
        z2 = skeleton[i+1]->getZ() - skeleton[i]->getZ();
        for (int j = 0; j <= 23; j++) {
            neck.push_back(new Sphere(skeleton[i]->getX() + thicknessNeck*cos(M_PI/12*j)*(-z1),
                                      skeleton[i]->getY() + thicknessNeck*sin(M_PI/12*j),
                                      skeleton[i]->getZ() + thicknessNeck*cos(M_PI/12*j)*x1,
                                      R,10,tex_body));
            neck.push_back(new Sphere(skeleton[i]->getX() + x2/2.0 + thicknessNeck*cos(M_PI/12*j)*(-z1),
                                      skeleton[i]->getY() + thicknessNeck*sin(M_PI/12*j),
                                      skeleton[i]->getZ() + z2/2.0 + thicknessNeck*cos(M_PI/12*j)*x1,
                                      R,10,tex_body));

        }
        thicknessNeck = thicknessNeck - 3*R/((last-first)/2+1);
    }
    for (int i = (last+first)/2 + 1; i < last; i++) {
        x1 = skeleton[i]->getX() - skeleton[i-1]->getX();
        z1 = skeleton[i]->getZ() - skeleton[i-1]->getZ();
        x2 = skeleton[i+1]->getX() - skeleton[i]->getX();
        z2 = skeleton[i+1]->getZ() - skeleton[i]->getZ();
        for (int j = 0; j <= 23; j++) {
            neck.push_back(new Sphere(skeleton[i]->getX() + thicknessNeck*cos(M_PI/12*j)*(-z1),
                                      skeleton[i]->getY() + thicknessNeck*sin(M_PI/12*j),
                                      skeleton[i]->getZ() + thicknessNeck*cos(M_PI/12*j)*x1,
                                      R,10,tex_body));
            neck.push_back(new Sphere(skeleton[i]->getX() + x2/3.0 + thicknessNeck*cos(M_PI/12*j)*(-z1),
                                      skeleton[i]->getY() + thicknessNeck*sin(M_PI/12*j),
                                      skeleton[i]->getZ() + z2/3.0 + thicknessNeck*cos(M_PI/12*j)*x1,
                                      R,10,tex_body));
            neck.push_back(new Sphere(skeleton[i]->getX() + 2.0*x2/3.0 + thicknessNeck*cos(M_PI/12*j)*(-z1),
                                      skeleton[i]->getY() + thicknessNeck*sin(M_PI/12*j),
                                      skeleton[i]->getZ() + 2.0*z2/3.0 + thicknessNeck*cos(M_PI/12*j)*x1,
                                      R,10,tex_body));
        }
        thicknessNeck = thicknessNeck-2*R/(last-first);
    }
    x1 = skeleton[last]->getX() - skeleton[last-1]->getX();
    z1 = skeleton[last]->getZ() - skeleton[last-1]->getZ();
    for (int j = 0; j <= 23; j++) {
        neck.push_back(new Sphere(skeleton[last]->getX() + thicknessNeck*cos(M_PI/12*j)*(-z1),
                                  skeleton[last]->getY() + thicknessNeck*sin(M_PI/12*j),
                                  skeleton[last]->getZ() + thicknessNeck*cos(M_PI/12*j)*x1,
                                  R,10,tex_body));
    }
}

void Dragon::completePaw(std::vector<Sphere*>& paw, int first, int last) {
    float z0 = skeleton[indexBody]->getZ();
    float x1,x2,y2,z1,z2;
    float thicknessPaw = 2*R;
    int start = first;
    while (abs(skeleton[start]->getZ()-z0) < 5*R)
        start++;
    for (int i = start; i <= last-1; i++) {
            x1 = skeleton[i]->getX() - skeleton[i-1]->getX();
            z1 = skeleton[i]->getZ() - skeleton[i-1]->getZ();
            x2 = skeleton[i+1]->getX() - skeleton[i]->getX();
            y2 = skeleton[i+1]->getY() - skeleton[i]->getY();
            z2 = skeleton[i+1]->getZ() - skeleton[i]->getZ();
            for (int j = 0; j <= 23; j++) {
                paw.push_back(new Sphere(skeleton[i]->getX() + thicknessPaw*cos(M_PI/12*j)*(-z1),
                                         skeleton[i]->getY() + thicknessPaw*sin(M_PI/12*j),
                                         skeleton[i]->getZ() + thicknessPaw*cos(M_PI/12*j)*x1,
                                         R,10,tex_body));
                paw.push_back(new Sphere(skeleton[i]->getX() + x2/2.0 + thicknessPaw*cos(M_PI/12*j)*(-z1),
                                         skeleton[i]->getY() + y2/2.0 + thicknessPaw*sin(M_PI/12*j),
                                         skeleton[i]->getZ() + z2/2.0 + thicknessPaw*cos(M_PI/12*j)*x1,
                                         R,10,tex_body));

            }
            thicknessPaw = thicknessPaw - 2*R/(last-first+1);
    }
    y2 = skeleton[last]->getY();
    z2 = skeleton[last]->getZ();
    paw.push_back(new Sphere(skeleton[last]->getX(),
                             y2,
                             z2,
                             1.01*R, 10, tex_body));
    float r = 0.9*R;
    for (int i = 0; i <= 16; i++) {
        float lastIndex = paw.size()-1;
        paw.push_back(new Sphere(paw[lastIndex]->getX() - paw[lastIndex]->getRadius()*cos(M_PI/180.0*10.0),
                                 paw[lastIndex]->getY() - paw[lastIndex]->getRadius()*sin(M_PI/180.0*10.0),
                                 z2,
                                 r, 10, tex_body));
        r = abs(r - 0.05*R);
    }
    r = 0.9*R;
    paw.push_back(new Sphere(skeleton[last]->getX(),
                             y2,
                             z2,
                             1.01*R, 10, tex_body));
    for (int i = 0; i <= 16; i++) {
        float lastIndex = paw.size()-1;
        paw.push_back(new Sphere(paw[lastIndex]->getX() - paw[lastIndex]->getRadius()*cos(M_PI/180.0*(-10.0)),
                                 paw[lastIndex]->getY() - paw[lastIndex]->getRadius()*sin(M_PI/180.0*(-10.0)),
                                 z2,
                                 r, 10, tex_body));
        r = abs(r - 0.05*R);
    }
    r = 0.9*R;
    paw.push_back(new Sphere(skeleton[last]->getX(),
                             y2,
                             z2,
                             1.01*R, 10, tex_body));
    for (int i = 0; i <= 16; i++) {
        float lastIndex = paw.size()-1;
        paw.push_back(new Sphere(paw[lastIndex]->getX() - paw[lastIndex]->getRadius()*cos(M_PI/180.0*40.0),
                                 paw[lastIndex]->getY() - paw[lastIndex]->getRadius()*sin(M_PI/180.0*40.0),
                                 z2,
                                 r, 10, tex_body));
        r = abs(r - 0.05*R);
    }
    r = 0.9*R;
    paw.push_back(new Sphere(skeleton[last]->getX(),
                             y2,
                             z2,
                             1.01*R, 10, tex_body));
    for (int i = 0; i <= 16; i++) {
        float lastIndex = paw.size()-1;
        paw.push_back(new Sphere(paw[lastIndex]->getX() - paw[lastIndex]->getRadius()*cos(M_PI/180.0*(-40.0)),
                                 paw[lastIndex]->getY() - paw[lastIndex]->getRadius()*sin(M_PI/180.0*(-40.0)),
                                 z2,
                                 r, 10, tex_body));
        r = abs(r - 0.05*R);
    }
}

////////////////////////////////////////////////////////////////////////
void Dragon::createPawLeftUp(float angle, int first, int last){
    skeleton.push_back(new Sphere(skeleton[indexBody]->getX(),
                                  skeleton[indexBody]->getY() + 2*R*cos(M_PI/180.0*angle),
                                  skeleton[indexBody]->getZ() + 2*R*sin(M_PI/180.0*angle),
                                  R,10,tex_skeleton));
    for (int i = first+1; i <= last; i++) {
        skeleton.push_back(new Sphere(skeleton[i-1]->getX(),
                                      skeleton[i-1]->getY() + 2*R*cos(M_PI/180.0*angle),
                                      skeleton[i-1]->getZ() + 2*R*sin(M_PI/180.0*angle),
                                      R,10,tex_skeleton));
    }
    completePaw(pawLeftUp, first, last);
}

/////////////////////////////////////////////////////////////////////////
void Dragon::createPawRightUp(float angle, int first, int last) {
    skeleton.push_back(new Sphere(skeleton[indexBody]->getX(),
                                  skeleton[indexBody]->getY() + 2*R*cos(M_PI/180.0*angle),
                                  skeleton[indexBody]->getZ() + 2*R*sin(M_PI/180.0*angle),
                                  R,10,tex_skeleton));
    for (int i = first+1; i <= last; i++) {
        skeleton.push_back(new Sphere(skeleton[i-1]->getX(),
                                      skeleton[i-1]->getY() + 2*R*cos(M_PI/180.0*angle),
                                      skeleton[i-1]->getZ() + 2*R*sin(M_PI/180.0*angle),
                                      R,10,tex_skeleton));
    }
    completePaw(pawRightUp, first, last);
}

/////////////////////////////////////////////////////////////////////////
void Dragon::createPawLeftDown(float angle, int first, int last){
    skeleton.push_back(new Sphere(skeleton[indexTail-1]->getX(),
                                  skeleton[indexTail-1]->getY() + 2*R*cos(M_PI/180.0*angle),
                                  skeleton[indexTail-1]->getZ() + 2*R*sin(M_PI/180.0*angle),
                                  R,10,tex_skeleton));
    for (int i = first+1; i <= last; i++) {
        skeleton.push_back(new Sphere(skeleton[i-1]->getX(),
                                      skeleton[i-1]->getY() + 2*R*cos(M_PI/180.0*angle),
                                      skeleton[i-1]->getZ() + 2*R*sin(M_PI/180.0*angle),
                                      R,10,tex_skeleton));
    }
    completePaw(pawLeftDown, first, last);
}

/////////////////////////////////////////////////////////////////////////
void Dragon::createPawRightDown(float angle, int first, int last){
    skeleton.push_back(new Sphere(skeleton[indexTail-1]->getX(),
                                  skeleton[indexTail-1]->getY() + 2*R*cos(M_PI/180.0*angle),
                                  skeleton[indexTail-1]->getZ() + 2*R*sin(M_PI/180.0*angle),
                                  R,10,tex_skeleton));
    for (int i = first+1; i <= last; i++) {
        skeleton.push_back(new Sphere(skeleton[i-1]->getX(),
                                      skeleton[i-1]->getY() + 2*R*cos(M_PI/180.0*angle),
                                      skeleton[i-1]->getZ() + 2*R*sin(M_PI/180.0*angle),
                                      R,10,tex_skeleton));
    }
    completePaw(pawRightDown, first, last);
}

/////////////////////////////////////////////////////////////////////////////
void Dragon::drawSkeleton(){
    for(std::vector<Sphere*>::iterator it = skeleton.begin() ; it != skeleton.end(); it++){
        Sphere* s = *it;
        s->draw();
    }
}

/////////////////////////////////////////////////////////////////////////////////
void Dragon::collisionParticleGround(Sphere *p)
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

///////////////////////////////////////////////////////////////////////////////////
void Dragon::keyPressEvent(QKeyEvent *e, Viewer & viewer){
    // Get event modifiers key
    const Qt::KeyboardModifiers modifiers = e->modifiers();

        /* Controls added for Lab Session 6 "Physicall Modeling" */
     if ((e->key()==Qt::Key_E) && (modifiers==Qt::NoButton)) {
        fly_force -= 50*gravity;
        /*toggleGravity = !toggleGravity;
        setGravity(toggleGravity);
        viewer.displayMessage("Set gravity to "
            + (toggleGravity ? QString("true") : QString("false")));*/

    } else if ((e->key()==Qt::Key_D) && (modifiers==Qt::NoButton)) {
                fly_force[2] -= 50;
        /*toggleViscosity = !toggleViscosity;
        setViscosity(toggleViscosity);
        viewer.displayMessage("Set viscosity to "
            + (toggleViscosity ? QString("true") : QString("false")));*/

    } else if ((e->key()==Qt::Key_Q) && (modifiers==Qt::NoButton)) {
        /*toggleCollisions = !toggleCollisions;
        setCollisionsDetection(toggleCollisions);
        viewer.displayMessage("Detects collisions "
            + (toggleCollisions ? QString("true") : QString("false")));*/

    } else if ((e->key()==Qt::Key_F) && (modifiers==Qt::NoButton)) {
        // stop the animation, and reinit the scene
        /*viewer.stopAnimation();
        init(viewer);
        viewer.manipulatedFrame()->setPosition(getFixedParticlePosition());
        toggleGravity = true;
        toggleViscosity = true;
        toggleCollisions = true;*/
    } else if ((e->key()==Qt::Key_T) && (modifiers==Qt::NoButton)) {

    } else if ((e->key()==Qt::Key_Y) && (modifiers==Qt::NoButton)) {
    }
}

