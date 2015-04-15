#ifndef DEF_FIRESMOKE
#define DEF_FIRESMOKE

#include <vector>
#include "renderable.h"


class Particule {
	public:
		bool isActive();
		void setActive(bool active);

		double getLife();
		void setLife(double life);
		void incrLife(double incr);

		double getVelDis();
		void setVelDis(double velDis);

		qglviewer::Vec getColor();
		void setColor(qglviewer::Vec color);

		qglviewer::Vec getPos();
		void setPos(qglviewer::Vec pos);
		void incrPos(qglviewer::Vec incr);

		qglviewer::Vec getMovVec();
		void setMovVec(qglviewer::Vec movVec);
		void incrMovVec(qglviewer::Vec incr);

		qglviewer::Vec getGravity();
		void setGravity(qglviewer::Vec gravity);


	private:
		bool active;
		double life;
		double velDis;
		qglviewer::Vec color;
		qglviewer::Vec pos;
		qglviewer::Vec movVec;
		qglviewer::Vec gravity;

};



class FireSmoke : public Renderable {
	public:
		FireSmoke(bool firesmoke, qglviewer::Vec origin, int nbParticles = 1000);
		void init(Viewer &v);
		void draw();
		void animate();

		qglviewer::Vec getOrigin();
		void setOrigin(qglviewer::Vec origin);

	private:
		bool firesmoke;
		qglviewer::Vec origin;
		std::vector<Particule> particles;

		void initParticle(Particule &p);
		double alea(double min, double max);
};
#endif