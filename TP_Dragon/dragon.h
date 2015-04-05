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

private :
    void drawBody();
    void drawTail();

    Cylinder *c;
    TrapezeIsocele *t;
    TrapezeIsocele *t2;

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

    // load a single texture file to associate with a Textureid
    GLuint loadTexture(const char *filename, bool mipmap=false);


};

#endif // DRAGON_H
