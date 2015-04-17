#ifndef DRAGON_H
#define DRAGON_H
#include "renderable.h"
#include "shader.h"
#include "cylinder.h"
#include "trapezeIsocele.h"
#include "sphere.h"
#include "spring.h"
#include <vector>
#include <string>
#include <QImage>

class Dragon : public Renderable
{
public:
    Dragon();
    ~Dragon();
    void draw();
    void init(Viewer &v);
    void animate();
    void keyPressEvent(QKeyEvent*e, Viewer& viewer);
    void collisionParticleGround(Sphere *p);
    void collisionParticleParticle(Sphere *s1, Sphere *s2);

private:
    void drawBody(int first, int last);
    void drawTail(int first, int last);
    void drawNeck(int first, int last);
    // Draw the left claw of the left paw if leftPaw==true and leftClaw==true
    void drawClaw(bool leftPaw, bool leftClaw);
    // Draw the left paw if left==true else the right paw
    void drawPawLeftUp(int first, int last);
    void drawPawRightUp(int first, int last);
    void drawPawLeftDown(int first, int last);
    void drawPawRightDown(int first, int last);
    void drawWing(bool left);
    void createBody(int first, int last);
    void createTail(float angle, int first, int last);
    void createNeck(int first, int last);
    void completePaw(std::vector<Sphere*>& paw, int first, int last);
    void createPawLeftUp(float angle, int first, int last);
    void createPawRightUp(float angle, int first, int last);
    void createPawLeftDown(float angle, int first, int last);
    void createPawRightDown(float angle, int first, int last);
    void drawSkeleton();
    void drawSprings();
    void createWingR();
    void drawWingR();
    void meshWingR();
    void drawMeshWingR();

    std::vector<Sphere*> skeleton;
    std::vector<Sphere*> body;
    std::vector<Sphere*> tail;
    std::vector<Sphere*> neck;
    std::vector<Sphere*> pawLeftUp;
    std::vector<Sphere*> pawRightUp;
    std::vector<Sphere*> pawLeftDown;
    std::vector<Sphere*> pawRightDown;
    int nbSpheresBody;
    int nbSpheresTail;
    int nbSpheresNeck;
    int nbSpheresPaw;
    std::vector<Spring*> sprgSkel;
    std::vector<Spring*> sprgWing1R;
    int indexBody;
    int indexTail;
    int indexNeck;
    int indexPawLeftUp;
    int indexPawRightUp;
    int indexPawLeftDown;
    int indexPawRightDown;
    int indexLastPawRightDown;
    float thicknessBody;
    float R;

    TrapezeIsocele *t;
    TrapezeIsocele *t2;

    float first_angle_wing;
    bool first_angle_wing_up;

    float second_angle_wing;
    bool second_angle_wing_up;

    float third_angle_wing;
    bool third_angle_wing_up;

    float time_wing1;
    float time_wing2;
    float time_wing3;

    bool fly_up;
    float dist_flyx;
    float dist_flyy;
    float dist_flyz;

    double mass;

    Sphere*** wingR1;

    Sphere* dragPart;

    //std::vector<Renderable> component_list;
    void drawBasePlane(float size);

    // textures used in this practical
    GLuint tex_body,tex_field,tex_feu,tex_skeleton;

    // texture unit shader binding
    GLint texture0;

    // texture coordinate bindings
    GLint texcoord0;

    // shader program
    ShaderProgram program;

    // load a single texture file to associate with a Textureid
    GLuint loadTexture(const char *filename, bool mipmap=false);

    // Inits lighting
    void initLighting();


};

#endif // DRAGON_H
