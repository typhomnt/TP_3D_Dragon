#ifndef DEF_SPHERE
#define DEF_SPHERE

#include "shader.h"
#include "renderable.h"
#include <QGLViewer/qglviewer.h>

class Sphere : public Renderable
{
	public:
		Sphere();
        Sphere(float x, float y, float z, float radius,float mass=10.0,GLint tex=0,bool fix=false, bool col=true);
        Sphere(qglviewer::Vec pos, qglviewer::Vec vel, float radius, float mass=10.0,GLint tex=0,bool fix=false, bool col=true);
		void init(Viewer &v);
		void draw();
        void generate(float radius);

        float getRadius();
        float getX();
        float getY();
        float getZ();

        void setRadius(float radius);
        void setMass(float m);
        void setX(float x);
        void setY(float y);
        void setZ(float z);

        void operator=(const Sphere& s);

		void setTexture(GLint id);

		bool getCollisions();
		void setCollisions(bool enable);

        const qglviewer::Vec & getPosition() const;
        const qglviewer::Vec & getVelocity() const;
        float getMass() const;
        float getInvMass() const;

        void setPosition(const qglviewer::Vec &pos);
        void setVelocity(const qglviewer::Vec &vel);
        void incrPosition(const qglviewer::Vec &pos);
        void incrVelocity(const qglviewer::Vec &vel);

        void setColor(float r, float g, float b, float a);
        void setFixed(bool b);
        bool getFixed() const;
	private:
		float radius;
        float x;
        float y;
        float z;

        qglviewer::Vec position;
        qglviewer::Vec velocity;
        float mass;
        float invMass; // the inverse of the mass is also stored

		ShaderProgram program;

		GLint tex;
		GLint texture0;

		void initLighting();

		bool toggleCollisions;

        bool colored;

        bool fixed;

        float r;
        float g;
        float b;
        float a;

};
#endif
