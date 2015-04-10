#ifndef DEF_SPHERE
#define DEF_SPHERE

#include "shader.h"
#include "renderable.h"
#include <QGLViewer/qglviewer.h>

class Sphere : public Renderable
{
	public:
		Sphere();
		void init(Viewer &v);
		void draw();
		void generate(float radius);

		void setTexture(GLint id);

		bool getCollisions();
		void setCollisions(bool enable);

	private:
		float radius;

		ShaderProgram program;

		GLint tex;
		GLint texture0;

		void initLighting();

		bool toggleCollisions;

};
#endif