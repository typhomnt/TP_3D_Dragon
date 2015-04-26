#include "dragon.h"
#include "glCheck.h"
#include "vec.h"
#include "light.h"
#include "material.h"
#include "hermite.h"
#include <cmath>
#include <QKeyEvent>
#include <stdlib.h>
#include <time.h>

#include <iostream>
#include <cstdio>
#include <stdexcept>
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
static  qglviewer::Vec groundPosition = qglviewer::Vec(0.0, 0.0, 0.0);
static  qglviewer::Vec groundNormal = qglviewer::Vec(0.0, 0.0, 1.0);
static float rebound = 0;
static qglviewer::Vec fly_force = qglviewer::Vec(0,0,0);
static float k = 800;
static float amort = 500;
static float lo;
static float nbw1 = 11;
static float nbw2 = 11;
static qglviewer::Vec wing1root;
static qglviewer::Vec wing1vel = qglviewer::Vec(0,0,0);
static qglviewer::Vec wing2root;
static qglviewer::Vec wing2vel = qglviewer::Vec(0,0,0);
static qglviewer::Vec initForces = qglviewer::Vec(0,0,0);
static float k1 = 200;
static float amort1 = 0;
static float lo1;
static float meshStep = 3.0;
static float wr = 0.2;
static float flyStep = M_PI/2;
static qglviewer::Vec  wingForce = qglviewer::Vec(0,0,0);
static float lo2;
static int tp = 1;
static int mod = 25;
static int tw = 1;
static float coeffw = -1;
static float walkstep = 0.05;
static float tks = 0.03;
static float fact = 1;
static float tailAngle = M_PI/16;
static float neckAngle = M_PI/16;
static qglviewer::Vec diffBody ;
static qglviewer::Vec diffNeck ;
static qglviewer::Vec diffTail ;
static qglviewer::Vec diffPawLU;
static qglviewer::Vec diffPawRU;
static qglviewer::Vec diffPawLD;
static qglviewer::Vec diffPawRD;

// Tableau permettant de calculer le mouvement de chaque sphère de la queue
static std::vector< std::vector<qglviewer::Vec> > hermiteQueue;

static int dtQueue = 0;

// Tableau permettant de calculer le mouvement de chaque sphère de la tete
static std::vector< std::vector<qglviewer::Vec> > hermiteTete;
static bool retourQueue = false;

static int dtTete = 0;

///////////////////////////////////////////////////////////////////////////////
Dragon::Dragon() {
    srand (time(NULL));
    // R = 0.5 pour Omid
    R = 0.1;
    lo=R/2;
    nbSpheresBody = 15;
    nbSpheresTail = 30;
    nbSpheresNeck = 15;
    nbSpheresPaw = 13;
    nbSpheresContourBody = 16;
    nbSpheresContourTail = 16;
    nbSpheresContourNeck = 12;
    nbSpheresContourPaw = 12;
    lo1=1/meshStep;
    lo2 = 0;
    indexBody = 0;
    indexTail = indexBody + nbSpheresBody;
    indexNeck = indexTail + nbSpheresTail;
    indexPawLeftUp = indexNeck + nbSpheresNeck;
    indexPawRightUp = indexPawLeftUp + nbSpheresPaw;
    indexPawLeftDown = indexPawRightUp + nbSpheresPaw;
    indexPawRightDown = indexPawLeftDown + nbSpheresPaw;
    indexLastPawRightDown = indexPawRightDown + nbSpheresPaw-1;
    indexHead = indexPawRightDown + nbSpheresPaw;
    thicknessBody = 4*R;
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
    fly_up = false;
    fly_down = true;
    walk = false;
    take_off = false;
    paw1w = false;
    paw2w = false;
    paw3w = false;
    paw4w = false;
    stopw = false;
    dist_flyx = 0.1;
    dist_flyy = 0.1;
    dist_flyz = 0.1;
    createBody(indexBody, indexTail-1);
    createTail(indexTail, indexNeck-1);
    createNeck(indexNeck, indexPawLeftUp-1);
    createPawLeftUp(-70, indexPawLeftUp, indexPawRightUp-1);
    createPawRightUp(-110, indexPawRightUp, indexPawLeftDown-1);
    createPawLeftDown(-70, indexPawLeftDown, indexPawRightDown-1);
    createPawRightDown(-110, indexPawRightDown, indexLastPawRightDown);
    createHead(indexHead);
    wing1root = skeleton[7]->getPosition() + qglviewer::Vec (0,5*R,5*R);
    wing2root = skeleton[7]->getPosition() + qglviewer::Vec (0,-5*R,5*R);
    nbSpheresWings1 = 20;
    nbSpheresWings2 = 30;
    nbSpheresWings3 = 26;
    nbSpheresWings4 = 22;
    nbSpheresWings5 = 20;
    nbSpheresWings6 = 8;
    indexWing2 = nbSpheresWings1;
    indexWing3 = indexWing2 + nbSpheresWings2;
    indexWing4 = indexWing3 + nbSpheresWings3;
    indexWing5 = indexWing4 + nbSpheresWings4;
    indexWing6 = indexWing5 + nbSpheresWings5;

    createWingR();
    //meshWingR();
    createWingL();
    //meshWingL();
    this->firesmoke = new FireSmoke(true, qglviewer::Vec(1,1,1), 10000);
    this->dust = new FireSmoke(false,qglviewer::Vec(1,1,1), 5000,true);
    this->grass = new Grass(2,100,20);
    this->mount = new Mountain(25,80,qglviewer::Vec(0,0,0));

    this->skybox = new Skybox(50.0, texture0, texcoord0);

    hermiteQueue = std::vector< std::vector<qglviewer::Vec> >(nbSpheresTail);
    this->moveQueue = false;
    hermiteTete = std::vector< std::vector<qglviewer::Vec> >(nbSpheresNeck + skeleton.size() - indexHead);
    this->moveNeck = false;
}


///////////////////////////////////////////////////////////////////////////////
Dragon::~Dragon() {
    delete dragPart;

    delete firesmoke;
    delete dust;
    delete grass;
    delete skybox;

    for(int i = 0; i < (int)skeleton.size(); i++){
        Sphere* s = skeleton[i];
        for(std::vector<Sphere*>::iterator it2 = skeleton[i]->getContour().begin() ; it2 != skeleton[i]->getContour().end(); it2++){
            Sphere* s2 = *it2;
            delete s2;
        }
        delete s;
    }
    for(std::vector<Sphere*>::iterator it = wingLeft.begin() ; it != wingLeft.end(); it++){
        Sphere* s = *it;
        delete s;
    }
    for(std::vector<Sphere*>::iterator it = wingRight.begin() ; it != wingRight.end(); it++){
        Sphere* s = *it;
        delete s;
    }

}


///////////////////////////////////////////////////////////////////////////////
void Dragon::init(Viewer &v) {
    // load textures
    tex_skeleton = loadTexture("images/texture_drag1.png");
    tex_field = loadTexture("images/field1.jpg");
    tex_body = loadTexture("images/peau.png");
    tex_feu = loadTexture("images/feu1.jpg");
    tex_aile = loadTexture("images/ailes3.jpg");
    tex_eye = loadTexture("images/oeil.jpeg");

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
    for(int i = 0; i < (int)skeleton.size(); i++){
        Sphere* s = skeleton[i];
        for(std::vector<Sphere*>::iterator it = skeleton[i]->getContour().begin() ; it != skeleton[i]->getContour().end(); it++){
            Sphere* s = *it;
            if (s->estTexturee())
                s->setTexture(tex_body);
            else
                s->setColor(254,150,160,1);
            s->init(v);
        }
        if (i >= indexHead) {
            if ((i == indexEyeLeft) || (i == indexEyeRight)) {
                s->setTexture(tex_eye);
            } else {
                s->setTexture(tex_body);
            }
        }
        s->init(v);
    }
    for(std::vector<Sphere*>::iterator it = wingLeft.begin() ; it != wingLeft.end(); it++){
        Sphere* s = *it;
        s->setTexture(tex_aile);
        s->init(v);
    }
    for(std::vector<Sphere*>::iterator it = wingRight.begin() ; it != wingRight.end(); it++){
        Sphere* s = *it;
        s->setTexture(tex_aile);
        s->init(v);
    }
    int somme = 0;
    for(int i = 0; i < (int)skeleton.size(); i++){
        somme++;
        Sphere* s = skeleton[i];
        for(std::vector<Sphere*>::iterator it = skeleton[i]->getContour().begin() ; it != skeleton[i]->getContour().end(); it++){
            somme++;
        }
    }
    somme = somme + wingLeft.size() + wingRight.size();
    printf("nb total = %i\n", somme);

    grass->init(v);
    mount->build();
    skybox->setProgram(program);
    skybox->init(v);
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

/////////////////////////////////////////////////////////////////////
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

    /*
    diffBody = skeleton[indexBody]->getPosition();
    diffNeck = skeleton[indexNeck]->getPosition();
    diffTail = skeleton[indexTail]->getPosition();
    diffPawLD = skeleton[indexPawLeftDown]->getPosition();
    diffPawLU = skeleton[indexPawLeftUp]->getPosition();
    diffPawRD = skeleton[indexPawRightDown]->getPosition();
    diffPawRU = skeleton[indexPawRightUp]->getPosition();
    skeleton[indexBody]->setMass(10);
    for(int i = 1 ; i < nbw1 ; i++){
        for(int j = 0 ; j < nbw1 ; j++){
            if(wingR1[i][j] != NULL){
                wingR1[i][j]->setZ(wingR1[i][j]->getZ() + 1.0*(sin(0.1*fact + (float)i*0.1)  - sin(0.1*(fact - 0.5) + (float)i*0.1)));
                wingR1[i][j]->setFixed(true);
            }
        }
    }
    for(int i = 0 ; i < nbw2 ; i++){
        for(int j = 0 ; j < nbw2 ; j++){
            if(wingR2[j][i] != NULL){
                wingR2[j][i]->setZ(wingR2[j][i]->getZ() + 1.0*(sin((float)(i+nbw1 -0.97)*0.1 + 0.1*fact)  - sin((float)(i+nbw1-0.97)*0.1 + 0.1*(fact - 0.5))));
                wingR2[j][i]->setFixed(true);
            }
        }
    }
    for(int i = 1 ; i < nbw1 ; i++){
        for(int j = 0 ; j < nbw1 ; j++){
            if(wingL1[i][j] != NULL){
                wingL1[i][j]->setZ(wingL1[i][j]->getZ() + 1.0*(sin(0.1*fact + (float)i*0.1)  - sin(0.1*(fact - 0.5) + (float)i*0.1)));
                wingL1[i][j]->setFixed(true);
            }
        }
    }
    for(int i = 0 ; i < nbw2 ; i++){
        for(int j = 0 ; j < nbw2 ; j++){
            if(wingL2[j][i] != NULL){
                wingL2[j][i]->setZ(wingL2[j][i]->getZ() + 1.0*(sin((float)(i+nbw1 -0.97)*0.1 + 0.1*fact)  - sin((float)(i+nbw1-0.97)*0.1 + 0.1*(fact - 0.5))));
                wingL2[j][i]->setFixed(true);
            }
        }
    }
    std::map<const Sphere *, qglviewer::Vec> forces;
    for(std::vector<Sphere*>::iterator it = skeleton.begin() ; it != skeleton.end(); ++it){
        Sphere* s = *it;
        forces[s] = initForces;
        if(!s->getFixed() && !walk && !take_off)
            forces[s] += gravity * s->getMass();
    }

    for(int i = 0 ; i < nbw1 ; i++){
        for(int j = 0 ; j < nbw1 ; j++){
            if(wingR1[i][j] != NULL)
                forces[wingR1[i][j]] = initForces;
        }
    }
    for(int i = 0 ; i < nbw2 ; i++){
        for(int j = 0 ; j < nbw2 ; j++){
            if(wingR2[i][j] != NULL)
                forces[wingR2[i][j]] = initForces;
        }
    }
    for(std::vector<Spring*>::iterator it = sprgSkel.begin() ; it != sprgSkel.end(); it++){
        Spring* s = *it;
        qglviewer::Vec f12 = s->getCurrentForce();
        if(fly_up || fly_down){
            if(!s->getParticle1()->getFixed())
                forces[s->getParticle1()] += f12;
            if(!s->getParticle2()->getFixed())
                forces[s->getParticle2()] -= f12;
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
        if(fly_up||fly_down){
        s->incrVelocity(dt*s->getInvMass()*forces[s]);
        s->incrPosition(dt*s->getVelocity());
        if(s->getCollisions())
            if(collisionParticleGround(s)){
                s->setFixed(true);
                fly_up = false;
                fly_down = false;
                s->setVelocity(initForces);
            }
        }
    }
    fly_force[0] = 0;
    fly_force[1] = 0;
    fly_force[2] = 0;
    /*if(!walk && !take_off && !fly_up || fly_down){
        for(unsigned int i = 0; i < skeleton.size(); ++i) {
            for(unsigned int j = 0; j < i; ++j){
                    Sphere *s1 = skeleton[i];
                    Sphere *s2 = skeleton[j];
                    if(s1->getCollisions() && s2->getCollisions())
                        collisionParticleParticle(s1,s2);
            }
        }
    }*/
    /*
    tp++;*/
    std::vector<qglviewer::Vec> diff;
    for(int i = 0 ; i < skeleton.size() ; i++)
        diff.push_back(skeleton[i]->getPosition());
    if(walk)
        walking();
    for(int i = 0 ; i < skeleton.size() ; i++)
        diff[i] -= skeleton[i]->getPosition();
    updateHermit(diff);
    /*
    if(take_off){
        take_off = false;
        stopw = true;
        for(int i = indexBody ; i <= indexLastPawRightDown  ; i++){
            skeleton[i]->setVelocity(qglviewer::Vec(0,0,5));
        }
    }*/
    firesmoke->setOrigin(qglviewer::Vec(skeleton[indexHead + 3]->getX() - R,skeleton[indexHead + 3]->getY() + R,skeleton[indexHead + 3]->getZ() + R));
    if (firesmoke->isActive())
        firesmoke->animate();
    if (dust->isActive())
        dust->animate();
    fly_up = false;
    fact+=1.5;


    //Roation de la queue
    if (moveQueue)
        moveTail();
    //Rotation du cou
    if (moveNeck)
        moveNeckHead();
    /*
    //
    updateWingPos();
    diffBody -= skeleton[indexBody]->getPosition();
    diffNeck -= skeleton[indexNeck]->getPosition();
    diffTail -= skeleton[indexTail]->getPosition();
    diffPawLD -= skeleton[indexPawLeftDown]->getPosition();
    diffPawLU -= skeleton[indexPawLeftUp]->getPosition();
    diffPawRD -= skeleton[indexPawRightDown]->getPosition();
    diffPawRU -= skeleton[indexPawRightUp]->getPosition();
    //updateDrag();
    */
}

void Dragon::walking(){
    if(paw1w){
        dust->inactivate();
        dust->setOrigin(skeleton[indexPawRightDown -1]->getPosition());
        if(tw%10 == 0){
            coeffw = -coeffw;
            if(tw%20 != 0)
                tw++;
        }
        if(tw%20 == 0){
            //coeffw = -coeffw;
            paw1w = false;
            paw2w = true;
            tw = 1;
            dust->activate();
        }
        for(int i = indexPawLeftDown ; i < indexPawRightDown ; i++){
            skeleton[i]->setZ(skeleton[i]->getZ() - walkstep/2*coeffw);
            skeleton[i]->setX(skeleton[i]->getX() - walkstep);
            //skeleton[i]->setY(skeleton[i]->getY() - walkstep);
        }
        for(int i = indexBody ; i < indexPawLeftUp ; i++){
            skeleton[i]->setX(skeleton[i]->getX() - walkstep/4);
            //skeleton[i]->setY(skeleton[i]->getY() - walkstep/4);
        }
        for(int i = indexHead ; i < skeleton.size(); i++)
            skeleton[i]->setX(skeleton[i]->getX() - walkstep/4);
    }
    else if(paw2w){
        dust->inactivate();
        dust->setOrigin(skeleton[indexPawRightUp -1]->getPosition());
        if(tw%10 == 0){
            coeffw = -coeffw;
            if(tw%20 != 0)
                tw++;
        }
        if(tw%20 == 0){
            //coeffw = -coeffw;
            paw2w = false;
            paw3w = true;
            tw = 1;
            dust->activate();
        }
        for(int i = indexPawLeftUp ; i < indexPawRightUp; i++){
            skeleton[i]->setZ(skeleton[i]->getZ() - walkstep/2*coeffw);
            skeleton[i]->setX(skeleton[i]->getX() - walkstep);
            //skeleton[i]->setY(skeleton[i]->getY() - walkstep);
        }
        for(int i = indexBody ; i < indexPawLeftUp ; i++){
            skeleton[i]->setX(skeleton[i]->getX() - walkstep/4);
            //skeleton[i]->setY(skeleton[i]->getY() - walkstep/4);
        }
        for(int i = indexHead ; i < skeleton.size() ; i++)
            skeleton[i]->setX(skeleton[i]->getX() - walkstep/4);
    }
    else if(paw3w){
        dust->inactivate();
        dust->setOrigin(skeleton[indexLastPawRightDown]->getPosition());
        if(tw%10 == 0){
            coeffw = -coeffw;
            if(tw%20 != 0)
                tw++;
        }
        if(tw%20 == 0){
            //coeffw = -coeffw;
            paw3w = false;
            paw4w = true;
            tw = 1;
            dust->activate();
        }
        for(int i = indexPawRightDown ; i <= indexLastPawRightDown ; i++){
            skeleton[i]->setZ(skeleton[i]->getZ() - walkstep/2*coeffw);
            skeleton[i]->setX(skeleton[i]->getX() - walkstep);
            //skeleton[i]->setY(skeleton[i]->getY() - walkstep);
        }
        for(int i = indexBody ; i < indexPawLeftUp ; i++){
            skeleton[i]->setX(skeleton[i]->getX() - walkstep/4);
            //skeleton[i]->setY(skeleton[i]->getY() - walkstep/4);
        }
        for(int i = indexHead ; i < skeleton.size(); i++)
            skeleton[i]->setX(skeleton[i]->getX() - walkstep/4);
    }
    else if(paw4w){
        dust->inactivate();
        dust->setOrigin(skeleton[indexPawLeftDown -1]->getPosition());
        if(tw%10 == 0){
            coeffw = -coeffw;
            if(tw%20 != 0)
                tw++;
        }
        if(tw%20 == 0){
            //coeffw = -coeffw;
            paw4w = false;
            paw1w = true;
            tw = 1;
            dust->activate();
        }
        for(int i = indexPawRightUp ; i < indexPawLeftDown; i++){
            skeleton[i]->setZ(skeleton[i]->getZ() - walkstep/2*coeffw);
            skeleton[i]->setX(skeleton[i]->getX() - walkstep);
            //skeleton[i]->setY(skeleton[i]->getY() - walkstep);
        }
        for(int i = indexBody ; i < indexPawLeftUp ; i++){
            skeleton[i]->setX(skeleton[i]->getX() - walkstep/4);
            //skeleton[i]->setY(skeleton[i]->getY() - walkstep/4);
        }
        for(int i = indexHead ; i < skeleton.size() ; i++)
            skeleton[i]->setX(skeleton[i]->getX() - walkstep/4);
    }
    tw++;
}

void Dragon::updateHermit(std::vector<qglviewer::Vec> diff){
    if(moveQueue){
        for(int i = indexTail ; i < indexNeck ; i++)
            for(int j = 0 ; j < hermiteQueue[0].size() ; j++)
                hermiteQueue[i - indexTail][j] -= diff[i];
    }
}

/*void Dragon::updateDrag(){
    for(std::vector<Sphere*>::iterator it = body.begin() ; it != body.end(); it++){
        Sphere* s = *it;
        s->incrPosition(-diffBody);
    }
    for(std::vector<Sphere*>::iterator it = tail.begin() ; it != tail.end(); it++){
        Sphere* s = *it;
        s->incrPosition(-diffTail);
    }
    for(std::vector<Sphere*>::iterator it = neck.begin() ; it != neck.end(); it++){
        Sphere* s = *it;
        s->incrPosition(-diffNeck);
    }
    for(std::vector<Sphere*>::iterator it = pawLeftUp.begin() ; it != pawLeftUp.end(); it++){
        Sphere* s = *it;
        s->incrPosition(-diffPawLU);
    }
    for(std::vector<Sphere*>::iterator it = pawRightUp.begin() ; it != pawRightUp.end(); it++){
        Sphere* s = *it;
        s->incrPosition(-diffPawRU);
    }
    for(std::vector<Sphere*>::iterator it = pawLeftDown.begin() ; it != pawLeftDown.end(); it++){
        Sphere* s = *it;
        s->incrPosition(-diffPawLD);
    }
    for(std::vector<Sphere*>::iterator it = pawRightDown.begin() ; it != pawRightDown.end(); it++){
        Sphere* s = *it;
        s->incrPosition(-diffPawRD);
    }
}*/

///////////////////////////////////////////////////////////////////////////////
void Dragon::draw(){


    GLCHECK(glUseProgram( (GLint)program ));
    //skybox->draw();
    glPushMatrix();
    drawBasePlane(50.0);
    glPopMatrix();
    GLCHECK(glActiveTexture(GL_TEXTURE0));
    GLCHECK(glBindTexture(GL_TEXTURE_2D, tex_skeleton));
    /*GLCHECK(glActiveTexture(GL_TEXTURE0));
    GLCHECK(glBindTexture(GL_TEXTURE_2D, tex_feu));
    GLCHECK(glUniform1i(texture0, 0));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);*/


    drawWingR();
    drawWingL();
    drawPart(indexBody, indexLastPawRightDown);
    drawHead(indexHead);

    /*glPushMatrix();
    mount->draw();
    glPopMatrix();*/

    GLCHECK(glUseProgram( 0 ));
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    if(firesmoke->isActive())
        firesmoke->draw();
    if(dust->isActive())
        dust->draw();
    glDisable(GL_BLEND);
}


///////////////////////////////////////////////////////////////////////////////
void Dragon::drawPart(int first, int last) {
    for (int i = first; i <= last; i++) {
        skeleton[i]->draw();
        for(std::vector<Sphere*>::iterator it = skeleton[i]->getContour().begin() ; it != skeleton[i]->getContour().end(); it++){
            Sphere* s = *it;
            s->draw();
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
void Dragon::drawHead(int first) {
    drawPart(first,skeleton.size()-1);
    for(std::vector<Tooth*>::iterator it = teeths.begin() ; it != teeths.end(); it++){
        Tooth* t = *it;
        t->draw();
    }
}

/////////////////////////////////////////////////////////////////////
void Dragon::createBody(int first, int last){
    height = 30*R;
    skeleton.push_back(new Sphere(0,0,height,R,10,0));
    for (int i = first+1; i <= last; i++) {
        skeleton.push_back(new Sphere(skeleton[i-1]->getX()+2*R,0,skeleton[0]->getZ(),R,10,tex_skeleton));
        sprgSkel.push_back(new Spring(skeleton[i-1],skeleton[i],k,lo,amort));
    }
    nbSpheresContourBody = (int)(float)(M_PI*thicknessBody/R)+1;
    for (int i = first; i <= last; i++) {
        std::vector<Sphere*> &contour = skeleton[i]->getContour();
        for (int j = 0; j <= nbSpheresContourBody-1; j++) {
            contour.push_back(new Sphere(skeleton[i]->getX(),
                                         skeleton[i]->getY() + thicknessBody*cos(2*M_PI/nbSpheresContourBody*j),
                                         skeleton[i]->getZ() + thicknessBody*sin(2*M_PI/nbSpheresContourBody*j),
                                         R));
            contour.push_back(new Sphere(skeleton[i]->getX() + R,
                                         skeleton[i]->getY() + thicknessBody*cos(2*M_PI/nbSpheresContourBody*j),
                                         skeleton[i]->getZ() + thicknessBody*sin(2*M_PI/nbSpheresContourBody*j),
                                         R));
            if ((2*M_PI/nbSpheresContourBody*j > 13*M_PI/12.0) && (2*M_PI/nbSpheresContourBody*j < 23*M_PI/12.0)){
                contour[contour.size()-2]->doitEtreTexturee(false);
                contour[contour.size()-1]->doitEtreTexturee(false);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////
void Dragon::createTail(int first, int last){
    float x0 = skeleton[first-1]->getX();
    float z0 = skeleton[first-1]->getZ();
    for (int i = first; i <= last; i++) {
        float x = x0 + 2*R*(i-first+1);
        skeleton.push_back(new Sphere(x,
                                      0,
                                      z0 + (x-x0)*(x-x0)/(80.0*R),
                                      R,10,tex_skeleton));
        sprgSkel.push_back(new Spring(skeleton[i-1],skeleton[i],k,lo,amort));
    }
    float thicknessTail = thicknessBody;
    float x1,x2,z1,z2,norme;
    for (int i = first; i <= (last+first)/2; i++) {
        x1 = skeleton[i]->getX() - skeleton[i-1]->getX();
        z1 = skeleton[i]->getZ() - skeleton[i-1]->getZ();
        norme = sqrt(x1*x1+z1*z1);
        x1 = x1/norme;
        z1 = z1/norme;
        x2 = skeleton[i+1]->getX() - skeleton[i]->getX();
        z2 = skeleton[i+1]->getZ() - skeleton[i]->getZ();
        nbSpheresContourTail = (int)floor(M_PI*thicknessTail/R)+1;
        if (nbSpheresContourTail < 4)
            nbSpheresContourTail = 4;
        std::vector<Sphere*> &contour = skeleton[i]->getContour();
        for (int j = 0; j <= nbSpheresContourTail-1; j++) {
            contour.push_back(new Sphere(skeleton[i]->getX() + thicknessTail*cos(2*M_PI/nbSpheresContourTail*j)*(-z1),
                                         skeleton[i]->getY() + thicknessTail*sin(2*M_PI/nbSpheresContourTail*j),
                                         skeleton[i]->getZ() + thicknessTail*cos(2*M_PI/nbSpheresContourTail*j)*x1,
                                         R));
            contour.push_back(new Sphere(skeleton[i]->getX() + x2/2.0 + thicknessTail*cos(2*M_PI/nbSpheresContourTail*j)*(-z1),
                                         skeleton[i]->getY() + thicknessTail*sin(2*M_PI/nbSpheresContourTail*j),
                                         skeleton[i]->getZ() + z2/2.0 + thicknessTail*cos(2*M_PI/nbSpheresContourTail*j)*x1,
                                         R));
            if ((2*M_PI/nbSpheresContourTail*j > 2*M_PI/3.0) && (2*M_PI/nbSpheresContourTail*j < 4*M_PI/3.0)){
                contour[contour.size()-2]->doitEtreTexturee(false);
                contour[contour.size()-1]->doitEtreTexturee(false);
            }
        }
        thicknessTail = thicknessTail - 0.75*thicknessBody/((last-first)/2+1);
    }
    for (int i = (last+first)/2 + 1; i < last; i++) {
        x1 = skeleton[i]->getX() - skeleton[i-1]->getX();
        z1 = skeleton[i]->getZ() - skeleton[i-1]->getZ();
        norme = sqrt(x1*x1+z1*z1);
        x1 = x1/norme;
        z1 = z1/norme;
        x2 = skeleton[i+1]->getX() - skeleton[i]->getX();
        z2 = skeleton[i+1]->getZ() - skeleton[i]->getZ();
        nbSpheresContourTail = (int)floor(M_PI*thicknessTail/R)+1;
        if (nbSpheresContourTail < 4)
            nbSpheresContourTail = 4;
        std::vector<Sphere*> &contour = skeleton[i]->getContour();
        for (int j = 0; j <= nbSpheresContourTail-1; j++) {
            contour.push_back(new Sphere(skeleton[i]->getX() + thicknessTail*cos(2*M_PI/nbSpheresContourTail*j)*(-z1),
                                         skeleton[i]->getY() + thicknessTail*sin(2*M_PI/nbSpheresContourTail*j),
                                         skeleton[i]->getZ() + thicknessTail*cos(2*M_PI/nbSpheresContourTail*j)*x1,
                                         R));
            contour.push_back(new Sphere(skeleton[i]->getX() + x2/2.0 + thicknessTail*cos(2*M_PI/nbSpheresContourTail*j)*(-z1),
                                         skeleton[i]->getY() + thicknessTail*sin(2*M_PI/nbSpheresContourTail*j),
                                         skeleton[i]->getZ() + z2/2.0 + thicknessTail*cos(2*M_PI/nbSpheresContourTail*j)*x1,
                                         R));
            if ((2*M_PI/nbSpheresContourTail*j > 2.0*M_PI/3.0) && (2*M_PI/nbSpheresContourTail*j < 4.0*M_PI/3.0)){
                contour[contour.size()-2]->doitEtreTexturee(false);
                contour[contour.size()-1]->doitEtreTexturee(false);
            }
        }
        thicknessTail = thicknessTail-0.5*thicknessBody/(last-first);
    }
    x1 = skeleton[last]->getX() - skeleton[last-1]->getX();
    z1 = skeleton[last]->getZ() - skeleton[last-1]->getZ();
    norme = sqrt(x1*x1+z1*z1);
    x1 = x1/norme;
    z1 = z1/norme;
    nbSpheresContourTail = (int)floor(M_PI*thicknessTail/R)+1;
    if (nbSpheresContourTail < 4)
        nbSpheresContourTail = 4;
    std::vector<Sphere*> &contour = skeleton[last]->getContour();
    for (int j = 0; j <= nbSpheresContourTail-1; j++) {
        contour.push_back(new Sphere(skeleton[last]->getX() + thicknessTail*cos(2*M_PI/nbSpheresContourTail*j)*(-z1),
                                     skeleton[last]->getY() + thicknessTail*sin(2*M_PI/nbSpheresContourTail*j),
                                     skeleton[last]->getZ() + thicknessTail*cos(2*M_PI/nbSpheresContourTail*j)*x1,
                                     R));
        if ((2*M_PI/nbSpheresContourTail*j > 2*M_PI/3.0) && (2*M_PI/nbSpheresContourTail*j < 4.0*M_PI/3.0)){
            contour[contour.size()-1]->doitEtreTexturee(false);
        }
    }
}

/////////////////////////////////////////////////////////////////////
void Dragon::createNeck(int first, int last){
    skeleton.push_back(new Sphere(skeleton[indexBody]->getX()-2*R,
                                  0,
                                  skeleton[indexBody]->getZ(),
                                  R,10,tex_skeleton));
    sprgSkel.push_back(new Spring(skeleton[first],skeleton[0],k,R,amort));
    float x0 = skeleton[indexNeck]->getX();
    float z0 = skeleton[indexNeck]->getZ();
    for (int i = first+1; i <= last; i++) {
        float x = x0 - 2*R*(i-first);
        skeleton.push_back(new Sphere(x,
                                      0,
                                      z0 + (x-x0)*(x-x0)/(30.0*R),
                                      R,10,tex_skeleton));
        sprgSkel.push_back(new Spring(skeleton[i-1],skeleton[i],k,lo,amort));
    }
    float thicknessNeck = thicknessBody;
    float x1,x2,z1,z2,norme;
    nbSpheresContourNeck = (int)floor(M_PI*thicknessNeck/R)+3;
    if (nbSpheresContourNeck < 4)
        nbSpheresContourNeck = 4;
    std::vector<Sphere*> &contour = skeleton[first]->getContour();
    for (int j = 0; j <= nbSpheresContourNeck-1; j++) {
        contour.push_back(new Sphere(skeleton[first]->getX() - R,
                                     skeleton[first]->getY() + thicknessNeck*cos(2*M_PI/nbSpheresContourNeck*j),
                                     skeleton[first]->getZ() + thicknessNeck*sin(2*M_PI/nbSpheresContourNeck*j),
                                     R));
        contour.push_back(new Sphere(skeleton[first]->getX(),
                                     skeleton[first]->getY() + thicknessNeck*cos(2*M_PI/nbSpheresContourNeck*j),
                                     skeleton[first]->getZ() + thicknessNeck*sin(2*M_PI/nbSpheresContourNeck*j),
                                     R));
        contour.push_back(new Sphere(skeleton[first]->getX() + R,
                                     skeleton[first]->getY() + thicknessNeck*cos(2*M_PI/nbSpheresContourNeck*j),
                                     skeleton[first]->getZ() + thicknessNeck*sin(2*M_PI/nbSpheresContourNeck*j),
                                     R));
        if ((2*M_PI/nbSpheresContourNeck*j > 13*M_PI/12.0) && (2*M_PI/nbSpheresContourNeck*j < 23*M_PI/12.0)){
            contour[contour.size()-3]->doitEtreTexturee(false);
            contour[contour.size()-2]->doitEtreTexturee(false);
            contour[contour.size()-1]->doitEtreTexturee(false);
        }
    }
    thicknessNeck = thicknessNeck - 2*R/((last-first)/2+1);
    for (int i = first+1; i <= (last+first)/2; i++) {
        x1 = skeleton[i]->getX() - skeleton[i-1]->getX();
        z1 = skeleton[i]->getZ() - skeleton[i-1]->getZ();
        norme = sqrt(x1*x1+z1*z1);
        x1 = x1/norme;
        z1 = z1/norme;
        x2 = skeleton[i+1]->getX() - skeleton[i]->getX();
        z2 = skeleton[i+1]->getZ() - skeleton[i]->getZ();
        nbSpheresContourNeck = (int)floor(M_PI*thicknessNeck/R)+2;
        std::vector<Sphere*> &contour = skeleton[i]->getContour();
        for (int j = 0; j <= nbSpheresContourNeck-1; j++) {
            contour.push_back(new Sphere(skeleton[i]->getX() + thicknessNeck*cos(2*M_PI/nbSpheresContourNeck*j)*(-z1),
                                         skeleton[i]->getY() + thicknessNeck*sin(2*M_PI/nbSpheresContourNeck*j),
                                         skeleton[i]->getZ() + thicknessNeck*cos(2*M_PI/nbSpheresContourNeck*j)*x1,
                                         R));
            contour.push_back(new Sphere(skeleton[i]->getX() + x2/2.0 + thicknessNeck*cos(2*M_PI/nbSpheresContourNeck*j)*(-z1),
                                         skeleton[i]->getY() + thicknessNeck*sin(2*M_PI/nbSpheresContourNeck*j),
                                         skeleton[i]->getZ() + z2/2.0 + thicknessNeck*cos(2*M_PI/nbSpheresContourNeck*j)*x1,
                                         R));
            if ((2*M_PI/nbSpheresContourNeck*j <= M_PI/3.0) || (2*M_PI/nbSpheresContourNeck*j >= 5.0*M_PI/3.0)){
                contour[contour.size()-2]->doitEtreTexturee(false);
                contour[contour.size()-1]->doitEtreTexturee(false);
            }
        }
        thicknessNeck = thicknessNeck - 2*R/((last-first)/2+1);
    }
    for (int i = (last+first)/2 + 1; i < last; i++) {
        x1 = skeleton[i]->getX() - skeleton[i-1]->getX();
        z1 = skeleton[i]->getZ() - skeleton[i-1]->getZ();
        norme = sqrt(x1*x1+z1*z1);
        x1 = x1/norme;
        z1 = z1/norme;
        x2 = skeleton[i+1]->getX() - skeleton[i]->getX();
        z2 = skeleton[i+1]->getZ() - skeleton[i]->getZ();
        nbSpheresContourNeck = (int)floor(M_PI*thicknessNeck/R)+4;
        std::vector<Sphere*> &contour = skeleton[i]->getContour();
        for (int j = 0; j <= nbSpheresContourNeck-1; j++) {
            contour.push_back(new Sphere(skeleton[i]->getX() + thicknessNeck*cos(2*M_PI/nbSpheresContourNeck*j)*(-z1),
                                         skeleton[i]->getY() + thicknessNeck*sin(2*M_PI/nbSpheresContourNeck*j),
                                         skeleton[i]->getZ() + thicknessNeck*cos(2*M_PI/nbSpheresContourNeck*j)*x1,
                                         R));
            contour.push_back(new Sphere(skeleton[i]->getX() + x2/3.0 + thicknessNeck*cos(2*M_PI/nbSpheresContourNeck*j)*(-z1),
                                         skeleton[i]->getY() + thicknessNeck*sin(2*M_PI/nbSpheresContourNeck*j),
                                         skeleton[i]->getZ() + z2/3.0 + thicknessNeck*cos(2*M_PI/nbSpheresContourNeck*j)*x1,
                                         R));
            contour.push_back(new Sphere(skeleton[i]->getX() + 2.0*x2/3.0 + thicknessNeck*cos(2*M_PI/nbSpheresContourNeck*j)*(-z1),
                                         skeleton[i]->getY() + thicknessNeck*sin(2*M_PI/nbSpheresContourNeck*j),
                                         skeleton[i]->getZ() + 2.0*z2/3.0 + thicknessNeck*cos(2*M_PI/nbSpheresContourNeck*j)*x1,
                                         R));
            if ((2*M_PI/nbSpheresContourNeck*j <= M_PI/3.0) || (2*M_PI/nbSpheresContourNeck*j >= 5.0*M_PI/3.0)){
                contour[contour.size()-3]->doitEtreTexturee(false);
                contour[contour.size()-2]->doitEtreTexturee(false);
                contour[contour.size()-1]->doitEtreTexturee(false);
            }
        }
        thicknessNeck = thicknessNeck-R/(last-first);
    }
    x1 = skeleton[last]->getX() - skeleton[last-1]->getX();
    z1 = skeleton[last]->getZ() - skeleton[last-1]->getZ();
    norme = sqrt(x1*x1+z1*z1);
    x1 = x1/norme;
    z1 = z1/norme;
    nbSpheresContourNeck = (int)floor(M_PI*thicknessNeck/R)+4;
    std::vector<Sphere*> &contour2 = skeleton[last]->getContour();
    for (int j = 0; j <= nbSpheresContourNeck-1; j++) {
        contour2.push_back(new Sphere(skeleton[last]->getX() + thicknessNeck*cos(2*M_PI/nbSpheresContourNeck*j)*(-z1),
                                      skeleton[last]->getY() + thicknessNeck*sin(2*M_PI/nbSpheresContourNeck*j),
                                      skeleton[last]->getZ() + thicknessNeck*cos(2*M_PI/nbSpheresContourNeck*j)*x1,
                                      R));
        if ((2*M_PI/nbSpheresContourNeck*j <= M_PI/3.0) || (2*M_PI/nbSpheresContourNeck*j >= 5.0*M_PI/3.0)){
            contour2[contour2.size()-1]->doitEtreTexturee(false);
        }
    }
}

void Dragon::completePaw(int first, int last) {
    float z0 = skeleton[indexBody]->getZ();
    float x1,x2,y2,z1,z2,norme;
    float thicknessPaw = 0.4*thicknessBody;
    int start = first;
    while (abs(skeleton[start]->getZ()-z0) < thicknessBody-R)
        start++;
    for (int i = start; i <= last-1; i++) {
        x1 = skeleton[i]->getX() - skeleton[i-1]->getX();
        z1 = skeleton[i]->getZ() - skeleton[i-1]->getZ();
        norme = sqrt(x1*x1+z1*z1);
        x1 = x1/norme;
        z1 = z1/norme;
        x2 = skeleton[i+1]->getX() - skeleton[i]->getX();
        y2 = skeleton[i+1]->getY() - skeleton[i]->getY();
        z2 = skeleton[i+1]->getZ() - skeleton[i]->getZ();
        nbSpheresContourPaw = (int)floor(M_PI*thicknessPaw/R)+1;
        if (nbSpheresContourPaw < 4)
            nbSpheresContourPaw = 4;
        std::vector<Sphere*> &contour = skeleton[i]->getContour();
        for (int j = 0; j <= nbSpheresContourPaw-1; j++) {
            contour.push_back(new Sphere(skeleton[i]->getX() + thicknessPaw*cos(2*M_PI/nbSpheresContourPaw*j)*(-z1),
                                         skeleton[i]->getY() + thicknessPaw*sin(2*M_PI/nbSpheresContourPaw*j),
                                         skeleton[i]->getZ() + thicknessPaw*cos(2*M_PI/nbSpheresContourPaw*j)*x1,
                                         R,i));
            contour.push_back(new Sphere(skeleton[i]->getX() + x2/2.0 + thicknessPaw*cos(2*M_PI/nbSpheresContourPaw*j)*(-z1),
                                         skeleton[i]->getY() + y2/2.0 + thicknessPaw*sin(2*M_PI/nbSpheresContourPaw*j),
                                         skeleton[i]->getZ() + z2/2.0 + thicknessPaw*cos(2*M_PI/nbSpheresContourPaw*j)*x1,
                                         R,i));
            if ((2*M_PI/nbSpheresContourPaw*j <= M_PI/3.0) || (2*M_PI/nbSpheresContourPaw*j >= 5.0*M_PI/3.0)){
                contour[contour.size()-2]->doitEtreTexturee(false);
                contour[contour.size()-1]->doitEtreTexturee(false);
            }

        }
        thicknessPaw = thicknessPaw - 2*R/(last-first+1);
    }
    std::vector<Sphere*> &contour = skeleton[skeleton.size()-1]->getContour();
    y2 = skeleton[last]->getY();
    z2 = skeleton[last]->getZ();
    contour.push_back(new Sphere(skeleton[last]->getX(),
                                 y2,
                                 z2,
                                 1.01*R, 10, tex_body));
    float r = 0.9*R;
    for (int i = 0; i <= 16; i++) {
        float lastIndexcontour = contour.size()-1;
        contour.push_back(new Sphere(contour[lastIndexcontour]->getX() - contour[lastIndexcontour]->getRadius()*cos(M_PI/180.0*10.0),
                                     contour[lastIndexcontour]->getY() - contour[lastIndexcontour]->getRadius()*sin(M_PI/180.0*10.0),
                                     z2,
                                     r, 10, tex_body));
        r = abs(r - 0.05*R);
    }
    r = 0.9*R;
    contour.push_back(new Sphere(skeleton[last]->getX(),
                                 y2,
                                 z2,
                                 1.01*R, 10, tex_body));
    for (int i = 0; i <= 16; i++) {
        float lastIndexcontour = contour.size()-1;
        contour.push_back(new Sphere(contour[lastIndexcontour]->getX() - contour[lastIndexcontour]->getRadius()*cos(M_PI/180.0*(-10.0)),
                                     contour[lastIndexcontour]->getY() - contour[lastIndexcontour]->getRadius()*sin(M_PI/180.0*(-10.0)),
                                     z2,
                                     r, 10, tex_body));
        r = abs(r - 0.05*R);
    }
    r = 0.9*R;
    contour.push_back(new Sphere(skeleton[last]->getX(),
                                 y2,
                                 z2,
                                 1.01*R, 10, tex_body));
    for (int i = 0; i <= 16; i++) {
        float lastIndexcontour = contour.size()-1;
        contour.push_back(new Sphere(contour[lastIndexcontour]->getX() - contour[lastIndexcontour]->getRadius()*cos(M_PI/180.0*40.0),
                                     contour[lastIndexcontour]->getY() - contour[lastIndexcontour]->getRadius()*sin(M_PI/180.0*40.0),
                                     z2,
                                     r, 10, tex_body));
        r = abs(r - 0.05*R);
    }
    r = 0.9*R;
    contour.push_back(new Sphere(skeleton[last]->getX(),
                                 y2,
                                 z2,
                                 1.01*R, 10, tex_body));
    for (int i = 0; i <= 16; i++) {
        float lastIndexcontour = contour.size()-1;
        contour.push_back(new Sphere(contour[lastIndexcontour]->getX() - contour[lastIndexcontour]->getRadius()*cos(M_PI/180.0*(-40.0)),
                                     contour[lastIndexcontour]->getY() - contour[lastIndexcontour]->getRadius()*sin(M_PI/180.0*(-40.0)),
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
    sprgSkel.push_back(new Spring(skeleton[indexBody],skeleton[first],k,lo,amort));
    for (int i = first+1; i <= last; i++) {
        skeleton.push_back(new Sphere(skeleton[i-1]->getX(),
                                      skeleton[i-1]->getY() + 2*R*cos(M_PI/180.0*angle),
                                      skeleton[i-1]->getZ() + 2*R*sin(M_PI/180.0*angle),
                                      R,10,tex_skeleton));
        sprgSkel.push_back(new Spring(skeleton[i-1],skeleton[i],k,lo,amort));
    }
    completePaw(first, last);
}

/////////////////////////////////////////////////////////////////////////
void Dragon::createPawRightUp(float angle, int first, int last) {
    skeleton.push_back(new Sphere(skeleton[indexBody]->getX(),
                                  skeleton[indexBody]->getY() + 2*R*cos(M_PI/180.0*angle),
                                  skeleton[indexBody]->getZ() + 2*R*sin(M_PI/180.0*angle),
                                  R,10,tex_skeleton));
    sprgSkel.push_back(new Spring(skeleton[indexBody],skeleton[first],k,lo,amort));
    for (int i = first+1; i <= last; i++) {
        skeleton.push_back(new Sphere(skeleton[i-1]->getX(),
                                      skeleton[i-1]->getY() + 2*R*cos(M_PI/180.0*angle),
                                      skeleton[i-1]->getZ() + 2*R*sin(M_PI/180.0*angle),
                                      R,10,tex_skeleton));
        sprgSkel.push_back(new Spring(skeleton[i-1],skeleton[i],k,lo,amort));
    }
    completePaw(first, last);
}

/////////////////////////////////////////////////////////////////////////
void Dragon::createPawLeftDown(float angle, int first, int last){
    skeleton.push_back(new Sphere(skeleton[indexTail-1]->getX(),
                                  skeleton[indexTail-1]->getY() + 2*R*cos(M_PI/180.0*angle),
                                  skeleton[indexTail-1]->getZ() + 2*R*sin(M_PI/180.0*angle),
                                  R,10,tex_skeleton));
    sprgSkel.push_back(new Spring(skeleton[indexTail-1],skeleton[first],k,lo,amort));
    for (int i = first+1; i <= last; i++) {
        skeleton.push_back(new Sphere(skeleton[i-1]->getX(),
                                      skeleton[i-1]->getY() + 2*R*cos(M_PI/180.0*angle),
                                      skeleton[i-1]->getZ() + 2*R*sin(M_PI/180.0*angle),
                                      R,10,tex_skeleton));
    }
    completePaw(first, last);
}

/////////////////////////////////////////////////////////////////////////
void Dragon::createPawRightDown(float angle, int first, int last){
    skeleton.push_back(new Sphere(skeleton[indexTail-1]->getX(),
                                  skeleton[indexTail-1]->getY() + 2*R*cos(M_PI/180.0*angle),
                                  skeleton[indexTail-1]->getZ() + 2*R*sin(M_PI/180.0*angle),
                                  R,10,tex_skeleton));
    sprgSkel.push_back(new Spring(skeleton[indexTail-1],skeleton[first],k,lo,amort));
    for (int i = first+1; i <= last; i++) {
        skeleton.push_back(new Sphere(skeleton[i-1]->getX(),
                                      skeleton[i-1]->getY() + 2*R*cos(M_PI/180.0*angle),
                                      skeleton[i-1]->getZ() + 2*R*sin(M_PI/180.0*angle),
                                      R,10,tex_skeleton));
        sprgSkel.push_back(new Spring(skeleton[i-1],skeleton[i],k,lo,amort));
    }
    completePaw(first, last);
}

void Dragon::drawSprings(){
    glColor3f(0.0, 0.28, 1.0);
    glLineWidth(5.0);
    for(std::vector<Spring*>::iterator it = sprgSkel.begin() ; it != sprgSkel.end(); it++){
        Spring* s = *it;
        s->draw();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////
void Dragon::createHead(int first){
    float x0 = skeleton[indexPawLeftUp-1]->getX();
    float y0 = skeleton[indexPawLeftUp-1]->getY();
    float z0 = skeleton[indexPawLeftUp-1]->getZ();
    float dx = x0 - skeleton[indexPawLeftUp-2]->getX();
    float dz = z0 - skeleton[indexPawLeftUp-2]->getZ();
    float norme = sqrt(dx*dx + dz*dz);
    dx = dx/norme;
    dz = dz/norme;
    indexJawUp = first-1;
    //fin du cou;
    for (int i = 1; i <= 2; i++) {
        skeleton.push_back(new Sphere(x0 + 2*R*i*dx,
                                      y0,
                                      z0 + 2*R*i*dz,
                                      R));
        indexJawUp++;
    }
    indexJawDown = indexJawUp;
    x0 = skeleton[skeleton.size()-1]->getX();
    z0 = skeleton[skeleton.size()-1]->getZ();
    //machoire haute
    for (int i = 1; i <= 4; i++) {
        skeleton.push_back(new Sphere(x0 + R*i*dz,
                                      y0,
                                      z0 - R*i*dx,
                                      R));
        indexJawDown++;
    }
    int i_debutMachoireHaute = skeleton.size()-1;
    float x1 = skeleton[i_debutMachoireHaute]->getX();
    float z1 = skeleton[i_debutMachoireHaute]->getZ();
    for (int i = 1; i <= 4; i++) {
        skeleton.push_back(new Sphere(x1 + 2*R*i*dx,
                                      y0,
                                      z1 + 2*R*i*dz,
                                      R));
        indexJawDown++;
    }
    int i_boutMachHaute = (int)skeleton.size()-1;
    skeleton.push_back(new Sphere(skeleton[indexJawUp]->getX(),
                                  y0,
                                  skeleton[indexJawUp]->getZ(),
                                  R));
    indexJawDown++;
    for (int i = 1; i <= 4; i++) {
        skeleton.push_back(new Sphere(x0 - R*i*dz,
                                      y0,
                                      z0 + R*i*dx,
                                      R));
    }
    float x2 = skeleton[skeleton.size()-1]->getX();
    float z2 = skeleton[skeleton.size()-1]->getZ();
    for (int i = 1; i <= 4; i++) {
        skeleton.push_back(new Sphere(x2 + 2*R*i*dx,
                                      y0,
                                      z2 + 2*R*i*dz,
                                      R));
    }
    int i_boutMachBasse = skeleton.size()-1;
    float x,y,z,dy;
    for (int i = first; i < (int)skeleton.size(); i++) {
        x = skeleton[i]->getX();
        y = skeleton[i]->getY();
        z = skeleton[i]->getZ();
        dy = dx*dx + dz*dz;
        std::vector<Sphere*> &contour = skeleton[i]->getContour();
        if (((i - indexJawUp >= 0) && (i -indexJawUp <= 4)) || ((i - indexJawDown >= 0) && (i -indexJawDown <= 4))) {
            for (int j = -2; j <= 2; j++) {
                contour.push_back(new Sphere(x,
                                             y + R*j*dy,
                                             z,
                                             R));
            }
        } else {
            for (int j = -1; j <= 1; j++) {
                contour.push_back(new Sphere(x,
                                             y + 2*R*j*dy,
                                             z,
                                             R));
            }
        }
    }
    Sphere* s = skeleton[i_boutMachHaute]->getContour()[0];
    skeleton.push_back(new Sphere(s->getX() + 2*R*dz,
                                  s->getY(),
                                  s->getZ() - 2*R*dx,
                                  1.25*R));
    s = skeleton[i_boutMachHaute]->getContour()[2];
    skeleton.push_back(new Sphere(s->getX() + 2*R*dz,
                                  s->getY(),
                                  s->getZ() - 2*R*dx,
                                  1.25*R));
    for (int i = 0; i <= 2; i++) {
        s = skeleton[i_debutMachoireHaute + i];
        std::vector<Sphere*> &contour = s->getContour();
        if (i == 0) {
            for (int j = 0; j <= 4; j++) {
                Sphere* s2 = contour[j];
                for (int k = 1; k <= 2; k++) {
                    if (((j == 0) || (j == 4)) && (k == 1)) {
                        skeleton.push_back(new Sphere(s2->getX() + 2*R*k*dz,
                                                      s2->getY(),
                                                      s2->getZ() - 2*R*k*dx,
                                                      2.25*R));
                        if (j == 0) {
                            indexEyeLeft = skeleton.size()-1;
                        }
                        if (j == 4) {
                            indexEyeRight = skeleton.size()-1;
                        }
                    }
                    if ((i != 2) || (k == 1)) {
                        skeleton.push_back(new Sphere(s2->getX() + 2*R*k*dz,
                                                      s2->getY(),
                                                      s2->getZ() - 2*R*k*dx,
                                                      1.25*R));
                    }
                }
            }
        } else {
            for (int j = 0; j <= 2; j++) {
                Sphere* s2 = contour[j];
                for (int k = 1; k <= 2; k++) {
                    if ((i != 2) || (k == 1)) {
                        skeleton.push_back(new Sphere(s2->getX() + 2*R*k*dz,
                                                      s2->getY(),
                                                      s2->getZ() - 2*R*k*dx,
                                                      1.25*R));
                    }
                }
            }
        }
    }
    x = skeleton[first]->getX();
    y = skeleton[first]->getY();
    z = skeleton[first]->getZ();
    for (int i = -1; i <= 4; i++) {
        skeleton.push_back(new Sphere(x + 2*R*i*dz,
                                      y,
                                      z - 2*R*i*dx,
                                      R));
    }
    for (int i = 1; i <= 6; i++) {
        s = skeleton[skeleton.size()-i];
        x = s->getX();
        y = s->getY();
        z = s->getZ();
        std::vector<Sphere*> &contour = s->getContour();
        for (int j = -2; j <= 2; j++) {
            contour.push_back(new Sphere(x,
                                         y + j*2*R,
                                         z,
                                         1.25*R));
        }
    }
    for (int i = 0; i <= 2; i+=2) {
        s = skeleton[i_boutMachBasse-i]->getContour()[0];
        x = s->getX();
        y = s->getY();
        z = s->getZ();
        qglviewer::Vec v1(x-R*dx*sin(3*M_PI/4), y+R*cos(3*M_PI/4), z-R*dz*sin(3*M_PI/4));
        qglviewer::Vec v2(x-R*dx*sin(M_PI/4), y+R*cos(M_PI/4), z-R*dz*sin(M_PI/4));
        qglviewer::Vec v3(x-R*dx*sin(7*M_PI/4), y+R*cos(7*M_PI/4), z-R*dz*sin(7*M_PI/4));
        qglviewer::Vec v4(x-R*dx*sin(5*M_PI/4), y+R*cos(5*M_PI/4), z-R*dz*sin(5*M_PI/4));
        qglviewer::Vec v5(x + 3.5*R*dz,y,z - 3.5*R*dx);
        teeths.push_back(new Tooth(v1,v2,v3,v4,v5));
    }
    for (int i = 0; i <= 2; i+=2) {
        s = skeleton[i_boutMachBasse-i]->getContour()[2];
        x = s->getX();
        y = s->getY();
        z = s->getZ();
        qglviewer::Vec v1(x-R*dx*sin(3*M_PI/4), y+R*cos(3*M_PI/4), z-R*dz*sin(3*M_PI/4));
        qglviewer::Vec v2(x-R*dx*sin(M_PI/4), y+R*cos(M_PI/4), z-R*dz*sin(M_PI/4));
        qglviewer::Vec v3(x-R*dx*sin(7*M_PI/4), y+R*cos(7*M_PI/4), z-R*dz*sin(7*M_PI/4));
        qglviewer::Vec v4(x-R*dx*sin(5*M_PI/4), y+R*cos(5*M_PI/4), z-R*dz*sin(5*M_PI/4));
        qglviewer::Vec v5(x + 3.5*R*dz,y,z - 3.5*R*dx);
        teeths.push_back(new Tooth(v1,v2,v3,v4,v5));
    }
    for (int i = 1; i <= 3; i+=2) {
        s = skeleton[i_boutMachHaute-i]->getContour()[0];
        x = s->getX();
        y = s->getY();
        z = s->getZ();
        qglviewer::Vec v1(x-R*dx*sin(3*M_PI/4), y+R*cos(3*M_PI/4), z-R*dz*sin(3*M_PI/4));
        qglviewer::Vec v2(x-R*dx*sin(5*M_PI/4), y+R*cos(5*M_PI/4), z-R*dz*sin(5*M_PI/4));
        qglviewer::Vec v3(x-R*dx*sin(7*M_PI/4), y+R*cos(7*M_PI/4), z-R*dz*sin(7*M_PI/4));
        qglviewer::Vec v4(x-R*dx*sin(M_PI/4), y+R*cos(M_PI/4), z-R*dz*sin(M_PI/4));
        qglviewer::Vec v5(x - 3.5*R*dz,y,z + 3.5*R*dx);
        teeths.push_back(new Tooth(v1,v2,v3,v4,v5));
    }
    for (int i = 1; i <= 3; i+=2) {
        s = skeleton[i_boutMachHaute-i]->getContour()[2];
        x = s->getX();
        y = s->getY();
        z = s->getZ();
        qglviewer::Vec v1(x-R*dx*sin(3*M_PI/4), y+R*cos(3*M_PI/4), z-R*dz*sin(3*M_PI/4));
        qglviewer::Vec v2(x-R*dx*sin(5*M_PI/4), y+R*cos(5*M_PI/4), z-R*dz*sin(5*M_PI/4));
        qglviewer::Vec v3(x-R*dx*sin(7*M_PI/4), y+R*cos(7*M_PI/4), z-R*dz*sin(7*M_PI/4));
        qglviewer::Vec v4(x-R*dx*sin(M_PI/4), y+R*cos(M_PI/4), z-R*dz*sin(M_PI/4));
        qglviewer::Vec v5(x - 3.5*R*dz,y,z + 3.5*R*dx);
        teeths.push_back(new Tooth(v1,v2,v3,v4,v5));
    }
}

void Dragon::createWingR() {
    float x = skeleton[indexBody]->getX();
    float y = skeleton[indexBody]->getY() + thicknessBody;
    float z = skeleton[indexBody]->getZ();
    float dx = cos(M_PI/180.0*160);
    float dy = sin(M_PI/180.0*160);
    for (int i = 1; i <= nbSpheresWings1; i++) {
        wingRight.push_back(new Sphere(x + 2*R*i*dx,
                                       y + 2*R*i*dy,
                                       z,
                                       2*R));
    }
    x = wingRight[wingRight.size()-1]->getX();
    y = wingRight[wingRight.size()-1]->getY();
    for (int i = 1; i <= nbSpheresWings2; i++) {
        wingRight.push_back(new Sphere(x,
                                       y + 2*R*i,
                                       z,
                                       2*R));
    }
    dx = cos(M_PI/180.0*75.0);
    dy = sin(M_PI/180.0*75.0);
    for (int i = 1; i <= nbSpheresWings3; i++) {
        wingRight.push_back(new Sphere(x + 2*R*i*dx,
                                       y + 2*R*i*dy,
                                       z,
                                       2*R));
    }
    dx = cos(M_PI/180.0*60.0);
    dy = sin(M_PI/180.0*60.0);
    for (int i = 1; i <= nbSpheresWings4; i++) {
        wingRight.push_back(new Sphere(x + 2*R*i*dx,
                                       y + 2*R*i*dy,
                                       z,
                                       2*R));
    }
    dx = cos(M_PI/180.0*45.0);
    dy = sin(M_PI/180.0*45.0);
    for (int i = 1; i <= nbSpheresWings5; i++) {
        wingRight.push_back(new Sphere(x + 2*R*i*dx,
                                       y + 2*R*i*dy,
                                       z,
                                       2*R));
    }
    x = wingRight[nbSpheresWings1/3]->getX();
    y = wingRight[nbSpheresWings1/3]->getY();
    dx = cos(M_PI/180.0*45.0);
    dy = sin(M_PI/180.0*45.0);
    for (int i = 1; i <= nbSpheresWings6; i++) {
        wingRight.push_back(new Sphere(x + 2*R*i*dx,
                                       y + 2*R*i*dy,
                                       z,
                                       2*R));
    }
}

void Dragon::drawWingR() {
    for(std::vector<Sphere*>::iterator it = wingRight.begin() ; it != wingRight.end(); it++){
        Sphere* s = *it;
        s->draw();
    }
}

void Dragon::createWingL() {
    float x = skeleton[indexBody]->getX();
    float y = skeleton[indexBody]->getY() - thicknessBody;
    float z = skeleton[indexBody]->getZ();
    float dx = cos(M_PI/180.0*200);
    float dy = sin(M_PI/180.0*200);
    for (int i = 1; i <= nbSpheresWings1; i++) {
        wingLeft.push_back(new Sphere(x + 2*R*i*dx,
                                      y + 2*R*i*dy,
                                      z,
                                      2*R));
    }
    x = wingLeft[wingLeft.size()-1]->getX();
    y = wingLeft[wingLeft.size()-1]->getY();
    for (int i = 1; i <= nbSpheresWings2; i++) {
        wingLeft.push_back(new Sphere(x,
                                      y - 2*R*i,
                                      z,
                                      2*R));
    }
    dx = cos(-M_PI/180.0*75.0);
    dy = sin(-M_PI/180.0*75.0);
    for (int i = 1; i <= nbSpheresWings3; i++) {
        wingLeft.push_back(new Sphere(x + 2*R*i*dx,
                                      y + 2*R*i*dy,
                                      z,
                                      2*R));
    }
    dx = cos(-M_PI/180.0*60.0);
    dy = sin(-M_PI/180.0*60.0);
    for (int i = 1; i <= nbSpheresWings4; i++) {
        wingLeft.push_back(new Sphere(x + 2*R*i*dx,
                                      y + 2*R*i*dy,
                                      z,
                                      2*R));
    }
    dx = cos(-M_PI/180.0*45.0);
    dy = sin(-M_PI/180.0*45.0);
    for (int i = 1; i <= nbSpheresWings5; i++) {
        wingLeft.push_back(new Sphere(x + 2*R*i*dx,
                                      y + 2*R*i*dy,
                                      z,
                                      2*R));
    }
    x = wingLeft[nbSpheresWings1/3]->getX();
    y = wingLeft[nbSpheresWings1/3]->getY();
    dx = cos(-M_PI/180.0*45.0);
    dy = sin(-M_PI/180.0*45.0);
    for (int i = 1; i <= nbSpheresWings6; i++) {
        wingLeft.push_back(new Sphere(x + 2*R*i*dx,
                                      y + 2*R*i*dy,
                                      z,
                                      2*R));
    }

}

void Dragon::drawWingL() {
    for(std::vector<Sphere*>::iterator it = wingLeft.begin() ; it != wingLeft.end(); it++){
        Sphere* s = *it;
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

///////////////////////////////////////////////////////////////////////////////////
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

/*void Dragon::updateWingPos(){
    qglviewer::Vec diff = wingR1[0][0]->getPosition();
    wing1root = skeleton[7]->getPosition() + qglviewer::Vec (0,5*R,5*R);
    wingR1[0][0]->setPosition(wing1root);
    diff = diff - wingR1[0][0]->getPosition();
    for(int i = 0 ; i < nbw1 ; i++){
        for(int j = 0 ; j < nbw1 ; j++){
            if(i !=0 || j !=0)
                if(wingR1[i][j] != NULL)
                    wingR1[i][j]->incrPosition(-diff);
        }
    }
    for(int i = 0 ; i < nbw2 ; i++){
        for(int j = 0 ; j < nbw2 ; j++){
            if(wingR2[i][j] != NULL)
                wingR2[i][j]->incrPosition(-diff);
        }
    }
    qglviewer::Vec diff2 = wingL1[0][0]->getPosition();
    wing2root = skeleton[7]->getPosition() + qglviewer::Vec (0,-5*R,5*R);
    wingL1[0][0]->setPosition(wing2root);
    diff2 = diff2 - wingL1[0][0]->getPosition();
    for(int i = 0 ; i < nbw1 ; i++){
        for(int j = 0 ; j < nbw1 ; j++){
            if(i !=0 || j !=0)
                if(wingL1[i][j] != NULL)
                    wingL1[i][j]->incrPosition(-diff2);
        }
    }
    for(int i = 0 ; i < nbw2 ; i++){
        for(int j = 0 ; j < nbw2 ; j++){
            if(wingL2[i][j] != NULL)
                wingL2[i][j]->incrPosition(-diff2);
        }
    }
}*/

void Dragon::computeTail(float angle){
    for (int i = indexTail; i < indexNeck; i++) {
        std::vector<qglviewer::Vec> tmp = generateCtlPts(i, angle, 1, 4,indexTail);
        hermiteQueue[i-indexTail] = Hermite::generate(tmp, 0.05);
        angle *= 1.01;
    }
}

void Dragon::computeNeck(float angle){
    for (int i = indexNeck; i < indexPawLeftUp; i++) {
        std::vector<qglviewer::Vec> tmp = generateCtlPts(i, angle, 1, 4,indexNeck);
        hermiteTete[i-indexNeck] = Hermite::generate(tmp, 0.05);
        angle *= 1.01;
    }
    int beginHeadHermite = indexPawLeftUp - indexNeck;
    for (int i = beginHeadHermite; i < skeleton.size() - indexHead + beginHeadHermite; i++) {
        std::vector<qglviewer::Vec> tmp = generateCtlPts(i - beginHeadHermite + indexHead , angle, 1, 4,indexNeck);
        hermiteTete[i] = Hermite::generate(tmp, 0.05);
        //angle *= 1.01;
    }
}

void Dragon::keyPressEvent(QKeyEvent *e, Viewer & viewer){
    // Get event modifiers key
    const Qt::KeyboardModifiers modifiers = e->modifiers();

    /* Controls added for Lab Session 6 "Physicall Modeling" */
    if ((e->key()==Qt::Key_E) && (modifiers==Qt::NoButton)) {
        fly_up = true;
        fly_down = true;
        walk = false;
        take_off = false;
        fly_force -= 800*gravity;
        /*toggleGravity = !toggleGravity;
        setGravity(toggleGravity);
        viewer.displayMessage("Set gravity to "
            + (toggleGravity ? QString("true") : QString("false")));*/

    } else if ((e->key()==Qt::Key_N) && (modifiers==Qt::NoButton)) {
        // On met à jour le vecteur indiquant la position des sphères de la queue
        if (!this->moveNeck) {
            this->moveNeck = true;
            computeNeck(neckAngle);
        }
        else
            this->moveNeck = false;

    } else if ((e->key()==Qt::Key_Q) && (modifiers==Qt::NoButton)) {
        // On met à jour le vecteur indiquant la position des sphères de la queue
        if (!this->moveQueue) {
            this->moveQueue = true;
            computeTail(tailAngle);
        }
        else
            this->moveQueue = false;

    } else if ((e->key()==Qt::Key_F) && (modifiers==Qt::NoButton)) {
        // On active ou non le feu/fumée
        if (!firesmoke->isActive())
            firesmoke->activate();
        else
            firesmoke->inactivate();
    } else if ((e->key()==Qt::Key_T) && (modifiers==Qt::NoButton)) {
        if(walk || stopw){
            take_off = true;
            walk = false;
            stopw = false;
        }

    } else if ((e->key()==Qt::Key_M) && (modifiers==Qt::NoButton)) {
        if(walk){
            walk = false;
            stopw = true;
        }
        else if(!fly_up){
            walk = true;
            if(!stopw)
                paw1w = true;
            stopw = false;
        }
        take_off = false;
    }
}



///////////////////////////////////////////////////////////////////////////////
std::vector<qglviewer::Vec> Dragon::generateCtlPts(int i, double angle,
                                                   int xyz, int nbPts,int indexRoot) {
    // On récupère la sphère correspondant à l'indice i
    qglviewer::Vec v = skeleton[i]->getPosition();
    qglviewer::Vec origin = skeleton[indexRoot]->getPosition();

    // On crée le vecteur résultat
    std::vector<qglviewer::Vec> res;
    res.push_back(v);

    // A chaque itéation, on calcule la rotation du pt
    for (int i = 1; i < nbPts; i++) {
        qglviewer::Vec tmp = res[i-1] - origin;
        switch (xyz) {
        case 0:     // Rotation axe X
            tmp[0] = tmp[0];
            tmp[1] = tmp[1]*cos(angle) - tmp[2]*sin(angle);
            tmp[2] = tmp[1]*sin(angle) + tmp[2]*cos(angle);
            break;
        case 1:     // Rotation axe Y
            tmp[0] =  tmp[0]*cos(angle) - tmp[2]*sin(angle);
            tmp[1] =  tmp[1];
            tmp[2] = tmp[0]*sin(angle) + tmp[2]*cos(angle);
            break;
        case 2:     // Rotation axe Z
            tmp[0] = tmp[0]*cos(angle) - tmp[1]*sin(angle);
            tmp[1] = tmp[0]*sin(angle) + tmp[1]*cos(angle);
            tmp[2] = tmp[2];
            break;
        default:
            throw std::invalid_argument("generateCtlPts: saisie invalide");
            break;
        }
        tmp += origin;
        res.push_back(tmp);
    }

    return res;
}



///////////////////////////////////////////////////////////////////////////////
void Dragon::moveTail() {
    for (int i = indexTail; i < indexNeck; i++) {
        skeleton[i]->setPosition(hermiteQueue[i-indexTail][dtQueue]);
    }

    // Si on arrive à la fin du mouvement, on le fait dans l'autre sens
    if ((dtQueue + 1) % hermiteQueue[0].size() == 0)
        retourQueue = true;
    else if (dtQueue == 0)
        retourQueue = false;

    if (retourQueue)
        dtQueue--;
    else
        dtQueue++;
}

///////////////////////////////////////////////////////////////////////////////
void Dragon::moveNeckHead() {
    if(walk)
        walk = false;
    for (int i = indexNeck; i < indexPawLeftUp; i++)
        skeleton[i]->setPosition(hermiteTete[i-indexNeck][dtTete]);
    for(int i = indexHead ; i < skeleton.size() ; i++)
        skeleton[i]->setPosition(hermiteTete[i + (indexPawLeftUp - indexNeck) - indexHead][dtTete]);
    if ((dtTete + 1) % hermiteTete[0].size() == 0)
        moveNeck = false;
    dtTete++;
    /*// Si on arrive à la fin du mouvement, on le fait dans l'autre sens
    if ((dtQueue + 1) % hermiteQueue[0].size() == 0)
        retourQueue = true;
    else if (dtQueue == 0)
        retourQueue = false;

    if (retourQueue)
        dtQueue--;
    else
        dtQueue++;*/
}

