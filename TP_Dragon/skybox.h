#ifndef DEF_SKYBOX
#define DEF_SKYBOX

#include "renderable.h"
#include "shader.h"

class Skybox : public Renderable {

    public:
        Skybox(float size, GLint texture0, GLint texcoord0);
        void init(Viewer &v);
        void draw();

        void setProgram(GLint program);

    private:
        float size;         // Taille de la skybox
        GLint skyTex[6];    // Textures pour chaque face
        GLint texture0;     // Pour le fragment shader
        GLint texcoord0;    // Coordonnées de texture

        GLuint loadTexture(const char *filename, bool mipmap = false);

        GLint program;
};

#endif
