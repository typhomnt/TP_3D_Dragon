#ifndef DRAGON_H
#define DRAGON_H
#include "renderable.h"
#include "shader.h"
#include "cylinder.h"
#include "sphere.h"
#include "spring.h"
#include "firesmoke.h"
#include "grass.h"
#include "mountain.h"
#include "skybox.h"
#include "tooth.h"

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
    bool collisionParticleGround(Sphere *p);
    void collisionParticleParticle(Sphere *s1, Sphere *s2);

private:
    void drawHead(int first);
    void drawPart(int first, int last);

    void createBody(int first, int last);
    void completeTail(int first, int last, bool dessus);
    void createTail(int first, int last);
    void createNeck(int first, int last);
    void completePaw(int first, int last);
    void createPawLeftUp(float angle, int first, int last);
    void createPawRightUp(float angle, int first, int last);
    void createPawLeftDown(float angle, int first, int last);
    void createPawRightDown(float angle, int first, int last);
    void createHead(int first);
    void drawSprings();
    void createWingR();
    void drawWingR();
    void meshWingR();
    void drawMeshWingR();
    void createWingL();
    void drawWingL();
    void meshWingL();
    void drawMeshWingL();
    void updateWingPos(std::vector<qglviewer::Vec>  diff);
    void moveTail();
    void moveNeckHead();
    void updateDrag(std::vector<qglviewer::Vec> diff);
    void moveWings();
    void computeTail(float angle);
    void computeNeck(float angle);
    void computeWings(float angle);
    void walking();
    void updateHermit(std::vector<qglviewer::Vec> diff);
    void fly(float z);
    void drawWingMemb();

    std::vector<Sphere*> skeleton;
    int nbSpheresContourBody;
    int nbSpheresContourTail;
    int nbSpheresContourNeck;
    int nbSpheresContourPaw;
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
    int indexHead;
    int indexJawUp;
    int indexJawDown;
    int indexEyeLeft;
    int indexEyeRight;
    int indexNoseLeft;
    int indexNoseRight;
    float thicknessBody;
    float R;
    float height;
    std::vector<Tooth*> teeths;
    std::vector<Sphere*> wingLeft;
    std::vector<Sphere*> wingRight;
    std::vector<Sphere*> wingmemb;
    std::vector<Spring*> wingspring;
    int nbSpheresWings1;
    int nbSpheresWings2;
    int nbSpheresWings3;
    int nbSpheresWings4;
    int nbSpheresWings5;
    int nbSpheresWings6;
    int indexWing2;
    int indexWing3;
    int indexWing4;
    int indexWing5;
    int indexWing6;

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
    bool fly_down;
    bool walk;
    bool paw1w;
    bool paw2w;
    bool paw3w;
    bool paw4w;
    bool stopw;
    bool take_off;
    float dist_flyx;
    float dist_flyy;
    float dist_flyz;

    double mass;

    Sphere* dragPart;

    //std::vector<Renderable> component_list;
    void drawBasePlane(float size);

    // textures used in this practical

    GLuint tex_body,tex_field,tex_feu,tex_aile,tex_skeleton,tex_eye;



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

    FireSmoke *firesmoke;
    FireSmoke *dust;

    FireSmoke *smoke1;
    FireSmoke *smoke2;

    Grass *grass;
    Mountain *mount;
    Mountain *iced;


    Skybox *skybox;     // Skybox

    bool moveQueue;     // true si on veut bouger la queue; false sinon
    bool moveNeck;      //true si on veut bouger la tete; false sinon
    bool moveWing;      // true si on veut bouger les ailes; false sinon

    // Permet de générer les points de controle de la courbe d'Hermite
    std::vector<qglviewer::Vec> generateCtlPts(int i, double angle, int xyz,
                                                    int nbPts,int indexRoot);

};

#endif // DRAGON_H
