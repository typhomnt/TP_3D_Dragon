#include "dragon.h"
#include "glCheck.h"
#include "vec.h"
#include "light.h"
#include "material.h"
#include <cmath>
#include <QKeyEvent>
#include <stdlib.h>
#include <time.h>

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
static float rebound = 0;
static qglviewer::Vec fly_force = qglviewer::Vec(0,0,0);
static float k = 800;
static float amort = 500;
static float lo;
static float nbw1 = 10;
static float nbw2 = 8;
static qglviewer::Vec wing1root = qglviewer::Vec(2,2,2);
static qglviewer::Vec wing1vel = qglviewer::Vec(0,0,0);
static qglviewer::Vec initForces = qglviewer::Vec(0,0,0);
static float k1 = 500;
static float amort1 = 100;
static float lo1;
static float meshStep = 3.0;
static float wr = 0.2;
static qglviewer::Vec  wingForce = qglviewer::Vec(10,10,20);
static qglviewer::Vec  wingForce2 = qglviewer::Vec(0,10,20);
static qglviewer::Vec  wingForce3 = qglviewer::Vec(10,0,20);
static float lo2;
static int tp = 1;
static int mod = 2;

/*nouveau dragon*/

///////////////////////////////////////////////////////////////////////////////
Dragon::Dragon() {
    srand (time(NULL));
    t = new TrapezeIsocele(7.0,5.0,5.0,0.2);
    // Trapeze au bout des ailes
    t2 = new TrapezeIsocele(25.0/7.0,0.1,5.0,0.2);
    R = 0.1;
    lo=R/2;
    lo1=1/meshStep;
    lo2 = 0;
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
    originFire = qglviewer::Vec(1,1,1);
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
    fly_up = false;
    dist_flyx = 0.1;
    dist_flyy = 0.1;
    dist_flyz = 0.1;
    wingR1 = (Sphere***)malloc(nbw1*sizeof(Sphere**));
    for(int i = 0 ; i < nbw1 ; i++){
        wingR1[i] = (Sphere**)malloc(nbw1*sizeof(Sphere*));
    }
    for(int i = 0 ; i < nbw1 ; i++){
        for(int j = 0 ; j < nbw1 ; j++){
            wingR1[i][j] = NULL;
        }
    }
    wingR2 = (Sphere***)malloc(nbw2*sizeof(Sphere**));
    for(int i = 0 ; i < nbw2 ; i++){
        wingR2[i] = (Sphere**)malloc(nbw2*sizeof(Sphere*));
    }
    for(int i = 0 ; i < nbw2 ; i++){
        for(int j = 0 ; j < nbw2 ; j++){
            wingR2[i][j] = NULL;
        }
    }
    createBody();
    createTail();
    createNeck();
    createPawLeftUp(-70);
    createPawRightUp(-110);
    createPawLeftDown(-70);
    createPawRightDown(-110);
    createFire();
    createWingR();
    meshWingR();

    this->firesmoke = new FireSmoke(false, qglviewer::Vec(1,1,1), 50000);
}


///////////////////////////////////////////////////////////////////////////////
Dragon::~Dragon() {
    delete t;
    delete t2;
    delete dragPart;
    delete firesmoke;
}


///////////////////////////////////////////////////////////////////////////////
void Dragon::init(Viewer &v) {
    // load textures
    tex_body = loadTexture("images/texture_drag1.png");
    tex_field = loadTexture("images/field1.jpg");
    tex_feu = loadTexture("images/feu1.jpg");
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
    for(std::vector<Sphere*>::iterator it = fire.begin() ; it != fire.end(); it++){
        Sphere* s = *it;
        //s->setTexture(tex_feu);
        s->setColor(1,0,0,1);
        s->init(v);
    }
    for(int i = 0 ; i < nbw1 ; i++){
        for(int j= 0 ; j < nbw1 ; j++){
            if(wingR1[i][j] != NULL){
                wingR1[i][j]->setTexture(tex_body);
                wingR1[i][j]->init(v);
            }
        }
    }
    for(int i = 0 ; i < nbw2 ; i++){
        for(int j= 0 ; j < nbw2 ; j++){
            if(wingR2[i][j] != NULL){
                wingR2[i][j]->setTexture(tex_body);
                wingR2[i][j]->init(v);
            }
        }
    }
    //firesmoke->init(v);
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
    glClearColor(218.0 / 255.0, 200.0 / 255.0, 196.0 / 255.0, 1.0f);
}


///////////////////////////////////////////////////////////////////////////////
void Dragon::drawBasePlane(float size) {
    GLCHECK(glActiveTexture(GL_TEXTURE0));
    GLCHECK(glBindTexture(GL_TEXTURE_2D, tex_field));
    GLCHECK(glUseProgram( (GLint)program ));
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
    GLCHECK(glUseProgram( 0 ));
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
   /* if(sin(time_wing1) > 0.9)
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
    }*/
    if(tp%mod == 0){
        wingForce[2] = -wingForce[2];
        mod = 5;
    }
    std::map<const Sphere *, qglviewer::Vec> forces;
    for(std::vector<Sphere*>::iterator it = skeleton.begin() ; it != skeleton.end(); ++it){
        Sphere* s = *it;
        forces[s] = initForces;
        if(!s->getFixed())
            forces[s] += gravity * s->getMass();
    }
    wingR1[0][0]->setFixed(true);
    wingR1[0][1]->setFixed(true);
    wingR1[1][0]->setFixed(true);
    wingR1[1][1]->setFixed(true);
    for(int i = 0 ; i < nbw1 ; i++){
        for(int j = 0 ; j < nbw1 ; j++){
            forces[wingR1[i][j]] = initForces;
        }
    }
    for(int i = 0 ; i < nbw2 ; i++){
        for(int j = 0 ; j < nbw2 ; j++){
            forces[wingR2[i][j]] = initForces;
        }
    }
    for(std::vector<Spring*>::iterator it = sprgSkel.begin() ; it != sprgSkel.end(); it++){
        Spring* s = *it;
        qglviewer::Vec f12 = s->getCurrentForce();
        if(fly_up){
            if(!s->getParticle1()->getFixed())
                forces[s->getParticle1()] += f12;
            if(!s->getParticle2()->getFixed())
                forces[s->getParticle2()] -= f12;
        }
    }
    for(int i = 0 ; i < nbw1 ; i++){
        for(int j = 0 ; j < nbw1 ; j++){
            if(wingR1[i][j] != NULL)
                if(!wingR1[i][j]->getFixed()){
                    if(i <= j )
                        forces[wingR1[i][j]] += wingForce2;
                    else if(j <= i)
                        forces[wingR1[i][j]] += wingForce3;
                    else
                        forces[wingR1[i][j]] += wingForce;
                }
        }
    }
    for(std::vector<Spring*>::iterator it = sprgWing1R.begin() ; it != sprgWing1R.end(); it++){
        Spring* s = *it;
        qglviewer::Vec f12 = s->getCurrentForce();
        if(!s->getParticle1()->getFixed())
            forces[s->getParticle1()] += f12;
        if(!s->getParticle2()->getFixed())
            forces[s->getParticle2()] -= f12;
    }
    for(std::vector<Sphere*>::iterator it = skeleton.begin() ; it != skeleton.end(); ++it){
        Sphere* s = *it;
        forces[s] -= viscosity*s->getVelocity();
        forces[s] += fly_force ;
        if(fly_up){
        s->incrVelocity(dt*s->getInvMass()*forces[s]);
        s->incrPosition(dt*s->getVelocity());
        if(s->getCollisions())
            if(collisionParticleGround(s)){
                s->setFixed(true);
                fly_up = false;
                s->setVelocity(initForces);
            }
        }
    }
    for(int i = 0 ; i < nbw1 ; i++){
        for(int j = 0 ; j < nbw1 ; j++){
            wingR1[i][j]->incrVelocity(dt*wingR1[i][j]->getInvMass()*forces[wingR1[i][j]]);
            wingR1[i][j]->incrPosition(dt*wingR1[i][j]->getVelocity());
        }
    }

    for(int i = 0 ; i < nbw2 ; i++){
        for(int j = 0 ; j < nbw2 ; j++){
            wingR2[i][j]->incrVelocity(dt*wingR2[i][j]->getInvMass()*forces[wingR2[i][j]]);
            wingR2[i][j]->incrPosition(dt*wingR2[i][j]->getVelocity());
        }
    }

    fly_force[0] = 0;
    fly_force[1] = 0;
    fly_force[2] = 0;
    for(unsigned int i = 0; i < skeleton.size(); ++i) {
        for(unsigned int j = 0; j < i; ++j){
                Sphere *s1 = skeleton[i];
                Sphere *s2 = skeleton[j];
                if(s1->getCollisions() && s2->getCollisions())
                    collisionParticleParticle(s1,s2);

        }
    }
    for(unsigned int i = 0; i < nbw1; ++i) {
        for(unsigned int j = 0; j < nbw1; ++j){
            for(unsigned int k = 0; k < i ; k ++){
                for(unsigned int l = 0 ;l < j ; l++){
                    Sphere *s1 = wingR1[i][j];
                    Sphere *s2 = wingR1[k][l];
                    if(s1->getCollisions() && s2->getCollisions())
                        collisionParticleParticle(s1,s2);
                }
            }

        }
    }
    int i = 0;
    for(std::vector<Sphere*>::iterator it = fire.begin() ; it != fire.end(); ++it){
        Sphere* s = *it;
        if(i != 0){
            if(abs(s->getX() - originFire[0] + rand()/RAND_MAX - 0.5) > 0.5){
                s->setPosition(originFire);
            }
            else{
                s->incrPosition(dt*s->getVelocity());
            }
        }
        i++;
    }
    tp++;

    if (firesmoke->isActive())
        firesmoke->animate();
}

///////////////////////////////////////////////////////////////////////////////
void Dragon::draw(){

    /*
    GLCHECK(glUseProgram( (GLint)program ));*/
    glPushMatrix();
    drawBasePlane(50.0);
    glPopMatrix();
    
    /*GLCHECK(glActiveTexture(GL_TEXTURE0));
    GLCHECK(glBindTexture(GL_TEXTURE_2D, tex_feu));
    GLCHECK(glUniform1i(texture0, 0));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);*/

    drawWingR();
    glPushMatrix();
    drawBody();
    glPopMatrix();

    glPushMatrix();
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

    drawSprings();
    drawMeshWingR();
    glPopMatrix();

    GLCHECK(glUseProgram( 0 ));


    if(firesmoke->isActive())
        firesmoke->draw();

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
    skeleton.push_back(new Sphere(0,0,15,R,10,0));
    for (int i = 1; i <= 14; i++) {
        skeleton.push_back(new Sphere(skeleton[i-1]->getX()+2*R,0,skeleton[0]->getZ(),0.1,10,tex_body));
        sprgSkel.push_back(new Spring(skeleton[i-1],skeleton[i],k,lo,amort));
    }
}
void Dragon::createTail(){
    for (int i = 15; i <= 30; i++) {
        skeleton.push_back(new Sphere(skeleton[i-1]->getX()+2*R*cos(-M_PI/180.0*30),
                                      0,
                                      skeleton[i-1]->getZ()+2*R*sin(-M_PI/180.0*30),
                                      R,10,tex_body));
        sprgSkel.push_back(new Spring(skeleton[i-1],skeleton[i],k,lo,amort));
    }
}

void Dragon::createNeck(){
    skeleton.push_back(new Sphere(skeleton[0]->getX()-2*R, 0, skeleton[0]->getZ(), R,10,tex_body));
    sprgSkel.push_back(new Spring(skeleton[31],skeleton[0],k,R,amort));
    for (int i = 32; i <= 45; i++) {
        skeleton.push_back(new Sphere(skeleton[i-1]->getX()-2*R*cos(M_PI/180.0*30),
                                      0,
                                      skeleton[i-1]->getZ()+2*R*sin(M_PI/180.0*30),
                                      R,10,tex_body));
        sprgSkel.push_back(new Spring(skeleton[i-1],skeleton[i],k,lo,amort));
    }
}

void Dragon::createPawLeftUp(float angle){
    skeleton.push_back(new Sphere(skeleton[0]->getX(),
                                  skeleton[0]->getY() + 2*R*cos(M_PI/180.0*angle),
                                  skeleton[0]->getZ() + 2*R*sin(M_PI/180.0*angle),
                                  R,10,tex_body));
    sprgSkel.push_back(new Spring(skeleton[0],skeleton[46],k,lo,amort));
    for (int i = 47; i <= 55; i++) {
        skeleton.push_back(new Sphere(skeleton[i-1]->getX(),
                                      skeleton[i-1]->getY() + 2*R*cos(M_PI/180.0*angle),
                                      skeleton[i-1]->getZ() + 2*R*sin(M_PI/180.0*angle),
                                      R,10,tex_body));
        sprgSkel.push_back(new Spring(skeleton[i-1],skeleton[i],k,lo,amort));
    }
}
void Dragon::createPawRightUp(float angle){
    skeleton.push_back(new Sphere(skeleton[0]->getX(),
                                  skeleton[0]->getY() + 2*R*cos(M_PI/180.0*angle),
                                  skeleton[0]->getZ() + 2*R*sin(M_PI/180.0*angle),
                                  R,10,tex_body));
    sprgSkel.push_back(new Spring(skeleton[0],skeleton[56],k,lo,amort));
    for (int i = 57; i <= 65; i++) {
        skeleton.push_back(new Sphere(skeleton[i-1]->getX(),
                                      skeleton[i-1]->getY() + 2*R*cos(M_PI/180.0*angle),
                                      skeleton[i-1]->getZ() + 2*R*sin(M_PI/180.0*angle),
                                      R,10,tex_body));
        sprgSkel.push_back(new Spring(skeleton[i-1],skeleton[i],k,lo,amort));
    }
}
void Dragon::createPawLeftDown(float angle){
    skeleton.push_back(new Sphere(skeleton[14]->getX(),
                                  skeleton[14]->getY() + 2*R*cos(M_PI/180.0*angle),
                                  skeleton[14]->getZ() + 2*R*sin(M_PI/180.0*angle),
                                  R,10,tex_body));
    sprgSkel.push_back(new Spring(skeleton[14],skeleton[66],k,lo,amort));
    for (int i = 67; i <= 75; i++) {
        skeleton.push_back(new Sphere(skeleton[i-1]->getX(),
                                      skeleton[i-1]->getY() + 2*R*cos(M_PI/180.0*angle),
                                      skeleton[i-1]->getZ() + 2*R*sin(M_PI/180.0*angle),
                                      R,10,tex_body));
        sprgSkel.push_back(new Spring(skeleton[i-1],skeleton[i],k,lo,amort));
    }
}
void Dragon::createPawRightDown(float angle){
    skeleton.push_back(new Sphere(skeleton[14]->getX(),
                                  skeleton[14]->getY() + 2*R*cos(M_PI/180.0*angle),
                                  skeleton[14]->getZ() + 2*R*sin(M_PI/180.0*angle),
                                  R,10,tex_body));
    sprgSkel.push_back(new Spring(skeleton[14],skeleton[76],k,lo,amort));
    for (int i = 77; i <= 85; i++) {
        skeleton.push_back(new Sphere(skeleton[i-1]->getX(),
                                      skeleton[i-1]->getY() + 2*R*cos(M_PI/180.0*angle),
                                      skeleton[i-1]->getZ() + 2*R*sin(M_PI/180.0*angle),
                                      R,10,tex_body));
        sprgSkel.push_back(new Spring(skeleton[i-1],skeleton[i],k,lo,amort));
    }
}

void Dragon::drawSkeleton(){
    for(std::vector<Sphere*>::iterator it = skeleton.begin() ; it != skeleton.end(); it++){
        Sphere* s = *it;
        s->draw();
    }
}

void Dragon::drawSprings(){
    glColor3f(0.0, 0.28, 1.0);
    glLineWidth(5.0);
    for(std::vector<Spring*>::iterator it = sprgSkel.begin() ; it != sprgSkel.end(); it++){
        Spring* s = *it;
        s->draw();
    }
}

void Dragon::drawFire(){
    for(std::vector<Sphere*>::iterator it = fire.begin() ; it != fire.end(); it++){
        Sphere* s = *it;
        s->draw();
    }
}

void Dragon::createFire(){
    int nbrPart = 700;
    float varx = -5;
    qglviewer::Vec initPosF = originFire;
    qglviewer::Vec initVecF = qglviewer::Vec(varx,50,20);
    for(int i = 0 ; i < nbrPart ; i++){
        fire.push_back(new Sphere(initPosF,initVecF,R,0));
        initVecF[0] += 0.3;
        if(i < nbrPart/2){
            initVecF[1] += 0.2 + 10*rand()/RAND_MAX - 0.5;
            initVecF[2] -= 0.2 + 10*rand()/RAND_MAX - 0.5;
        }
        else{
            initVecF[1] -= 0.2 + 10*rand()/RAND_MAX - 0.5;
            initVecF[2] += 0.2 + 10*rand()/(RAND_MAX  - 2)- 0.5;
        }
        if(initVecF[0] > 5)
            initVecF[0] = varx;

    }
}

void Dragon::createWingR(){
    wingR1[0][0] = new Sphere(wing1root,wing1vel,wr);
    for(int i = 0 ; i < nbw1 ; i++){
        for(int j= 0 ; j < nbw1 ; j++){
            if(i != 0 || j != 0){
                if(i == 0){
                    wingR1[i][j] = new Sphere(wingR1[0][0]->getX(),wingR1[0][0]->getY() + (float)j/meshStep,wingR1[0][0]->getZ(),wr);
                }
                else if (j == 0){
                    wingR1[i][j] = new Sphere(wingR1[0][0]->getX() + (float)i/meshStep,wingR1[0][0]->getY(),wingR1[0][0]->getZ(),wr);
                }
                else{
                    wingR1[i][j] = new Sphere(wingR1[0][0]->getX() + (float)i/meshStep,wingR1[0][0]->getY() + (float)j/meshStep,wingR1[0][0]->getZ(),wr);
                }
            }
        }
    }
    wingR2[0][0] = new Sphere(wingR1[(int)nbw1 - 1][(int)nbw1 - 1]->getX(), wingR1[(int)nbw1 - 1][(int)nbw1 - 1]->getY(), wingR1[(int)nbw1 - 1][(int)nbw1 - 1]->getZ(),wr);
    for(int i = 0 ; i < nbw2 ; i++){
        for(int j= 0 ; j < nbw2 ; j++){
            if(i != 0 || j != 0){
                if(i == 0){
                    wingR2[i][j] = new Sphere(wingR2[0][0]->getX(),wingR2[0][0]->getY() + (float)j/meshStep,wingR2[0][0]->getZ(),wr);
                }
                else if (j == 0){
                    wingR2[i][j] = new Sphere(wingR2[0][0]->getX() - (float)i/meshStep,wingR2[0][0]->getY(),wingR2[0][0]->getZ(),wr);
                }
                else{
                    wingR2[i][j] = new Sphere(wingR2[0][0]->getX() - (float)i/meshStep,wingR2[0][0]->getY() + (float)j/meshStep,wingR2[0][0]->getZ(),wr);
                }
            }
        }
    }
}

void Dragon::drawWingR(){
    GLCHECK(glUseProgram(program));
    GLCHECK(glActiveTexture(GL_TEXTURE0));
    GLCHECK(glBindTexture(GL_TEXTURE_2D, tex_feu));
    GLCHECK(glUniform1i(texture0, 0));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBegin(GL_QUADS);
    for(int i = 0 ; i < nbw1 - 1 ; i++){
        for(int j= 0 ; j < nbw1 - 1 ; j++){
            //glNormal3f(0,0,1);
            glVertexAttrib2f(texcoord0, 0, 0);
            glVertex3f(wingR1[i][j]->getX(),wingR1[i][j]->getY(),wingR1[i][j]->getZ());
            glVertexAttrib2f(texcoord0, 0, 0);
            glVertex3f(wingR1[i][j+1]->getX(),wingR1[i][j+1]->getY(),wingR1[i][j+1]->getZ());
            glVertexAttrib2f(texcoord0, 0, 0);
            glVertex3f(wingR1[i+1][j+1]->getX(),wingR1[i+1][j+1]->getY(),wingR1[i+1][j+1]->getZ());
            glVertexAttrib2f(texcoord0, 0, 0);
            glVertex3f(wingR1[i+1][j]->getX(),wingR1[i+1][j]->getY(),wingR1[i+1][j]->getZ());
            //if(wingR1[i][j] != NULL)
              //  wingR1[i][j]->draw();
        }
    }
    glEnd();
    glBegin(GL_QUADS);
    for(int i = 0 ; i < nbw2 - 1 ; i++){
        for(int j= 0 ; j < nbw2 - 1 ; j++){
            //glNormal3f(0,0,2);
            glVertexAttrib2f(texcoord0, 0, 0);
            glVertex3f(wingR2[i][j]->getX(),wingR2[i][j]->getY(),wingR2[i][j]->getZ());
            glVertexAttrib2f(texcoord0, 0, 0);
            glVertex3f(wingR2[i][j+1]->getX(),wingR2[i][j+1]->getY(),wingR2[i][j+1]->getZ());
            glVertexAttrib2f(texcoord0, 0, 0);
            glVertex3f(wingR2[i+1][j+1]->getX(),wingR2[i+1][j+1]->getY(),wingR2[i+1][j+1]->getZ());
            glVertexAttrib2f(texcoord0, 0, 0);
            glVertex3f(wingR2[i+1][j]->getX(),wingR2[i+1][j]->getY(),wingR2[i+1][j]->getZ());
            //if(wingR2[i][j] != NULL)
              //  wingR2[i][j]->draw();
        }
    }
    glEnd();
    GLCHECK(glUseProgram( 0 ));
}

void Dragon::meshWingR(){
    for(int i = 0 ; i < nbw1 ; i++){
        for(int j= 0 ; j < nbw1 ; j++){
            if(wingR1[i][j] != NULL){
                if(i < nbw1 - 1){
                    if(wingR1[i+1][j] != NULL)
                        sprgWing1R.push_back(new Spring(wingR1[i][j],wingR1[i+1][j],k1,lo1,amort1));
                }
                if(j < nbw1 - 1){
                    if(wingR1[i][j+1] != NULL)
                    sprgWing1R.push_back(new Spring(wingR1[i][j],wingR1[i][j+1],k1,lo1,amort1));
                }
                if(i != nbw1 - 1 && j != nbw1 - 1){
                    if(wingR1[i+1][j+1] != NULL)
                    sprgWing1R.push_back(new Spring(wingR1[i][j],wingR1[i+1][j+1],k1,lo1,amort1));
                }
                if(i != nbw1 - 1 && j != 0){
                    if(wingR1[i+1][j-1] != NULL)
                    sprgWing1R.push_back(new Spring(wingR1[i][j],wingR1[i+1][j-1],k1,lo1,amort1));
                }
            }
        }
    }
    for(int i = 0 ; i < nbw2 ; i++){
        if(wingR2[i][0] != NULL && wingR1[(int)nbw1 - 1 - i][(int)nbw1-1] != NULL)
            sprgWing1R.push_back(new Spring(wingR2[i][0],wingR1[(int)nbw1- 1 - i][(int)nbw1-1],k1,lo2,amort1));
    }
    for(int i = 0 ; i < nbw2 ; i++){
        for(int j= 0 ; j < nbw2 ; j++){
            if(wingR2[i][j] != NULL){
                if(i < nbw2 - 1){
                    if(wingR2[i+1][j] != NULL)
                        sprgWing1R.push_back(new Spring(wingR2[i][j],wingR2[i+1][j],k1,lo1,amort1));
                }
                if(j < nbw2 - 1){
                    if(wingR2[i][j+1] != NULL)
                    sprgWing1R.push_back(new Spring(wingR2[i][j],wingR2[i][j+1],k1,lo1,amort1));
                }
                if(i != nbw2 - 1 && j != nbw2 - 1){
                    if(wingR2[i+1][j+1] != NULL)
                    sprgWing1R.push_back(new Spring(wingR2[i][j],wingR2[i+1][j+1],k1,lo1,amort1));
                }
                if(i != nbw2 - 1 && j != 0){
                    if(wingR2[i+1][j-1] != NULL)
                    sprgWing1R.push_back(new Spring(wingR2[i][j],wingR2[i+1][j-1],k1,lo1,amort1));
                }
            }
        }
    }
}

void Dragon::drawMeshWingR(){
    glColor3f(0.0, 0.28, 1.0);
    glLineWidth(5.0);
    for(std::vector<Spring*>::iterator it = sprgWing1R.begin() ; it != sprgWing1R.end(); it++){
        Spring* s = *it;
        s->draw();
    }
}

/////////////////////////////////////////////////////////////////////////////////
bool Dragon::collisionParticleGround(Sphere *p)
{
    // don't process fixed particles (ground plane is fixed)
    if (p->getInvMass() == 0)
        return false;

    // particle-plane distance
    double penetration = (p->getPosition() - groundPosition) * groundNormal;
    penetration -= p->getRadius();
    if (penetration >= 0)
        return false;

    // penetration velocity
    double vPen = p->getVelocity() * groundNormal;

    // updates position and velocity of the particle
    p->incrPosition(-penetration * groundNormal);
    p->incrVelocity(-(1 + rebound) * vPen * groundNormal);
    return true;
}

void Dragon::collisionParticleParticle(Sphere *s1, Sphere *s2)
{
    // don't process fixed particles
    if (s1->getMass() == 0 && s2->getMass() == 0)
        return;

    // particle-particle distance
    qglviewer::Vec n = s1->getPosition() - s2->getPosition();
    double penetration = sqrt(n*n);
    penetration -= (s1->getRadius() + s2->getRadius());
    if (penetration >= 0)
        return;
    n.normalize();
    double m1 = s1->getMass(), m2 = s2->getMass();

    qglviewer::Vec v1 = s1->getVelocity();
    qglviewer::Vec v1x = (n * v1) * n;
    qglviewer::Vec v1y = v1 - v1x;

    qglviewer::Vec v2 = s2->getVelocity();
    n = -n;
    qglviewer::Vec v2x = (n * v2) * n;
    qglviewer::Vec v2y = v2 - v2x;

    // new velocities
    if (m1 != 0)
        s1->setVelocity(-v1x * (m1-m2)/(m1+m2)  + v2x * (2*m2/(m1+m2)) + v1y);
    if (m2 != 0)
        s2->setVelocity(v1x * (2*m1)/(m1+m2) + v2x * ((m2-m1)/(m1+m2)) + v2y);

    if (m1 == 0)
        s2->setVelocity(-(1 + rebound) * s2->getVelocity());
    if (m2 == 0)
        s1->setVelocity((1 + rebound) * s1->getVelocity());
}



void Dragon::keyPressEvent(QKeyEvent *e, Viewer & viewer){
    // Get event modifiers key
    const Qt::KeyboardModifiers modifiers = e->modifiers();

        /* Controls added for Lab Session 6 "Physicall Modeling" */
     if ((e->key()==Qt::Key_E) && (modifiers==Qt::NoButton)) {
        fly_up = true;
        fly_force -= 1000*gravity;
        /*toggleGravity = !toggleGravity;
        setGravity(toggleGravity);
        viewer.displayMessage("Set gravity to "
            + (toggleGravity ? QString("true") : QString("false")));*/

    } else if ((e->key()==Qt::Key_D) && (modifiers==Qt::NoButton)) {
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
        if (!firesmoke->isActive())
            firesmoke->activate();
        else
            firesmoke->inactivate();
    } else if ((e->key()==Qt::Key_T) && (modifiers==Qt::NoButton)) {

    } else if ((e->key()==Qt::Key_Y) && (modifiers==Qt::NoButton)) {
    }
}

