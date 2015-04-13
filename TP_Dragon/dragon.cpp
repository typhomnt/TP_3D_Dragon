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
    R = 0.1;
    indexBody = 0;
    indexTail = 15;
    indexNeck = 31;
    indexPawLeftUp = 46;
    indexPawRightUp = 56;
    indexPawLeftDown = 66;
    indexPawRightDown = 76;
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
    createBody();
    createTail();
    createNeck();
    createPawLeftUp(-70);
    createPawRightUp(-110);
    createPawLeftDown(-70);
    createPawRightDown(-110);
    //drawSkeleton();
    //this->skeleton.push_back(dragPart);
}


///////////////////////////////////////////////////////////////////////////////
Dragon::~Dragon() {
    delete t;
    delete t2;
    delete dragPart;
}


///////////////////////////////////////////////////////////////////////////////
void Dragon::init(Viewer &v) {
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

    defaultGravity = qglviewer::Vec(0.0, 0.0, -10.0);
    gravity = defaultGravity;
    viscosity = 1.0;
    dt = 0.01;
    for(std::vector<Sphere*>::iterator it = skeleton.begin() ; it != skeleton.end(); it++){
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
    GLCHECK(glBindTexture(GL_TEXTURE_2D, tex_body));
    GLCHECK(glUniform1i(texture0, 0));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glPushMatrix();
    drawBody();
    glPopMatrix();

    /*glPushMatrix();
    drawTail();
    glPopMatrix();

    glPushMatrix();
    drawNeck();
    glPopMatrix();

    glPushMatrix();
    drawPawLeftUp(-70.0);
    glPopMatrix();

    glPushMatrix();
    drawPawRightUp(-110.0);
    glPopMatrix();

    glPushMatrix();
    drawPawLeftDown(-70.0);
    glPopMatrix();

    glPushMatrix();
    drawPawRightDown(-110.0);
    glPopMatrix();

    glPopMatrix();*/
    
    GLCHECK(glUseProgram( 0 ));
}


///////////////////////////////////////////////////////////////////////////////
void Dragon::drawBody() {
    skeleton[0]->draw();
    for (int i = 1; i <= 14; i++) {
        skeleton[i]->draw();
    }
}


///////////////////////////////////////////////////////////////////////////////
void Dragon::drawTail() {
    for (int i = 15; i <= 30; i++) {
        skeleton[i]->draw();
    }
}


///////////////////////////////////////////////////////////////////////////////
void Dragon::drawNeck() {
    skeleton[31]->draw();
    for (int i = 32; i <= 45; i++) {
        skeleton[i]->draw();
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
void Dragon::drawPawLeftUp(float angle) {
    skeleton[46]->draw();
    for (int i = 47; i <= 55; i++) {
        skeleton[i]->draw();
    }

}

///////////////////////////////////////////////////////////////////////////////
void Dragon::drawPawRightUp(float angle) {
    skeleton[56]->draw();
    for (int i = 57; i <= 65; i++) {
        skeleton[i]->draw();
    }

}

///////////////////////////////////////////////////////////////////////////////
void Dragon::drawPawLeftDown(float angle) {
    skeleton[66]->draw();
    for (int i = 67; i <= 75; i++) {
        skeleton[i]->draw();
    }

}

///////////////////////////////////////////////////////////////////////////////
void Dragon::drawPawRightDown(float angle) {
    skeleton[76]->draw();
    for (int i = 77; i <= 85; i++) {
        skeleton[i]->draw();
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

void Dragon::createBody(){
    skeleton.push_back(new Sphere(0,0,15,R));
    for (int i = 1; i <= 14; i++) {
        skeleton.push_back(new Sphere(skeleton[i-1]->getX()+2*R,0,skeleton[0]->getZ(),0.1,10,tex_body));
    }
}
void Dragon::createTail(){
    for (int i = 15; i <= 30; i++) {
        skeleton.push_back(new Sphere(skeleton[i-1]->getX()+2*R*cos(-M_PI/180.0*30),
                                      0,
                                      skeleton[i-1]->getZ()+2*R*sin(-M_PI/180.0*30),
                                      R,10,tex_body));
    }
}

void Dragon::createNeck(){
    skeleton.push_back(new Sphere(skeleton[0]->getX()-2*R, 0, skeleton[0]->getZ(), R,10,tex_body));
    for (int i = 32; i <= 45; i++) {
        skeleton.push_back(new Sphere(skeleton[i-1]->getX()-2*R*cos(M_PI/180.0*30),
                                      0,
                                      skeleton[i-1]->getZ()+2*R*sin(M_PI/180.0*30),
                                      R,10,tex_body));
    }
}

void Dragon::createPawLeftUp(float angle){
    skeleton.push_back(new Sphere(skeleton[0]->getX(),
                                  skeleton[0]->getY() + 2*R*cos(M_PI/180.0*angle),
                                  skeleton[0]->getZ() + 2*R*sin(M_PI/180.0*angle),
                                  R,10,tex_body));
    for (int i = 47; i <= 55; i++) {
        skeleton.push_back(new Sphere(skeleton[i-1]->getX(),
                                      skeleton[i-1]->getY() + 2*R*cos(M_PI/180.0*angle),
                                      skeleton[i-1]->getZ() + 2*R*sin(M_PI/180.0*angle),
                                      R,10,tex_body));
    }
}
void Dragon::createPawRightUp(float angle){
    skeleton.push_back(new Sphere(skeleton[0]->getX(),
                                  skeleton[0]->getY() + 2*R*cos(M_PI/180.0*angle),
                                  skeleton[0]->getZ() + 2*R*sin(M_PI/180.0*angle),
                                  R,10,tex_body));
    for (int i = 57; i <= 65; i++) {
        skeleton.push_back(new Sphere(skeleton[i-1]->getX(),
                                      skeleton[i-1]->getY() + 2*R*cos(M_PI/180.0*angle),
                                      skeleton[i-1]->getZ() + 2*R*sin(M_PI/180.0*angle),
                                      R,10,tex_body));
    }
}
void Dragon::createPawLeftDown(float angle){
    skeleton.push_back(new Sphere(skeleton[14]->getX(),
                                  skeleton[14]->getY() + 2*R*cos(M_PI/180.0*angle),
                                  skeleton[14]->getZ() + 2*R*sin(M_PI/180.0*angle),
                                  R,10,tex_body));
    for (int i = 67; i <= 75; i++) {
        skeleton.push_back(new Sphere(skeleton[i-1]->getX(),
                                      skeleton[i-1]->getY() + 2*R*cos(M_PI/180.0*angle),
                                      skeleton[i-1]->getZ() + 2*R*sin(M_PI/180.0*angle),
                                      R,10,tex_body));
    }
}
void Dragon::createPawRightDown(float angle){
    skeleton.push_back(new Sphere(skeleton[14]->getX(),
                                  skeleton[14]->getY() + 2*R*cos(M_PI/180.0*angle),
                                  skeleton[14]->getZ() + 2*R*sin(M_PI/180.0*angle),
                                  R,10,tex_body));
    for (int i = 77; i <= 85; i++) {
        skeleton.push_back(new Sphere(skeleton[i-1]->getX(),
                                      skeleton[i-1]->getY() + 2*R*cos(M_PI/180.0*angle),
                                      skeleton[i-1]->getZ() + 2*R*sin(M_PI/180.0*angle),
                                      R,10,tex_body));
    }
}

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

