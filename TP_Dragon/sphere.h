/**
 * @brief Classe permettant de gérer une sphère texturée ou non
 * @author Julien Daval - Maxime Garcia - Omid Ghorreshi
 */

#ifndef DEF_SPHERE
#define DEF_SPHERE

#include "shader.h"
#include "renderable.h"
#include <QGLViewer/qglviewer.h>

class Sphere : public Renderable
{
    public:
        /** Constructeurs **/
        Sphere();
        Sphere(float x, float y, float z, float radius, bool texturee=true, 
                    float mass=10.0,GLint tex=0,bool fix=true, bool col=true);
        Sphere(qglviewer::Vec pos, qglviewer::Vec vel, float radius, float mass=10.0, 
                    GLint tex=0,bool fix=true, bool col=true);


        /** Méthodes héritées de Renderable **/
		void init(Viewer &v);
		void draw();
        void generate(float radius);


        /** Accesseurs / Mutateurs **/
        float getRadius();
        float getX();
        float getY();
        float getZ();
        bool estTexturee();
        std::vector<Sphere*>& getContour();

        void setRadius(float radius);
        void setMass(float m);
        void setX(float x);
        void setY(float y);
        void setZ(float z);
        void doitEtreTexturee(bool t);

        void operator=(const Sphere& s);

		void setTexture(GLint id);

		bool getCollisions();
		void setCollisions(bool enable);

        const qglviewer::Vec  getPosition() const;
        const qglviewer::Vec  getVelocity() const;
        float getMass() const;
        float getInvMass() const;

        void setPosition(const qglviewer::Vec &pos);
        void setVelocity(const qglviewer::Vec &vel);
        void incrPosition(const qglviewer::Vec &pos);
        void incrVelocity(const qglviewer::Vec &vel);

        void setColor(GLubyte r, GLubyte g, GLubyte b, GLubyte a);
        void setFixed(bool b);
        bool getFixed() const;


	private:
		float radius;                 // Rayon
        float x;                      // Position de la sphère
        float y;
        float z;
        bool texturee;                // true si sphère texturée; false sinon

        qglviewer::Vec position;      // Position
        qglviewer::Vec velocity;      // Vitesse
        float mass;                   // Masse
        float invMass;                // Inverse de la masse

        std::vector<Sphere*> contour; // Contour

		ShaderProgram program;        // Shader program (shaders propres à la sphère)

		GLint tex;                    // Id de texture de la sphère
		GLint texture0;               // Id de texture (du vertex shader)

        // Couleur de la sphère (si colorée)
        GLubyte r;                  
        GLubyte g;
        GLubyte b;
        GLubyte a;

        bool toggleCollisions;        // true si on active les collisions
        bool colored;                 // true si la sphère est colorée (pas texturée)
        bool fixed;                   // true si la sphère est fixée


        /** Initialise la lumière **/
		void initLighting();          
};



#endif
