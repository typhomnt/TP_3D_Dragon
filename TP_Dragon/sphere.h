#ifndef DEF_SPHERE
#define DEF_SPHERE

#include "shader.h"
#include "renderable.h"
#include <QGLViewer/qglviewer.h>

class Sphere : public Renderable
{
	public:
		Sphere();
        Sphere(float x, float y, float z, float radius);
		void init(Viewer &v);
		void draw();
        void generate(float radius);

        float getRadius();
        float getX();
        float getY();
        float getZ();

        void setRadius(float radius);
        void setX(float x);
        void setY(float y);
        void setZ(float z);

        void operator=(const Sphere& s);

		void setTexture(GLint id);

		bool getCollisions();
		void setCollisions(bool enable);

	private:
		float radius;
        float x;
        float y;
        float z;

		ShaderProgram program;

		GLint tex;
		GLint texture0;

		void initLighting();

		bool toggleCollisions;

};
#endif
