#ifndef DRAGON_H
#define DRAGON_H
#include "renderable.h"
#include "shader.h"
#include "cylinder.h"
#include "trapezeIsocele.h"
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

private:
    void drawBody();
    void drawTail();
    void drawHead();
    // Draw the left claw of the left paw if leftPaw==true and leftClaw==true
    void drawClaw(bool leftPaw, bool leftClaw);
    // Draw the left paw if left==true else the right paw
    void drawPaw(bool left);
    void drawWing(bool left);

    Cylinder *c;
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

    //std::vector<Renderable> component_list;
    void drawBasePlane(float size);

    // textures used in this practical
    GLuint tex_body,tex_field;

    // texture unit shader binding
    GLint texture0;

    // texture coordinate bindings
    GLint texcoord0;

    // shader program
    ShaderProgram program;
    ShaderProgram lighting;

    // load a single texture file to associate with a Textureid
    GLuint loadTexture(const char *filename, bool mipmap=false);

    // Inits lighting
    void initLighting();


};

#endif // DRAGON_H
