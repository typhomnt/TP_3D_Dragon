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


	private:
		bool active;			// true si la particule est active, false sinon
		double life;			// temps de vie de la particule
		double velDis;			// vitesse de disparition
		qglviewer::Vec color;	// couleur de la particule
		qglviewer::Vec pos;		// position de la particule
		qglviewer::Vec movVec;	// vecteur de mouvement
};



class FireSmoke : public Renderable {
	public:
		FireSmoke(bool firesmoke, qglviewer::Vec origin, int nbParticles = 1000);
		void init(Viewer &v);
		void draw();
		void animate();

		qglviewer::Vec getOrigin();
		void setOrigin(qglviewer::Vec origin);

		bool isActive();
		void activate();
		void inactivate();

	private:
		bool firesmoke;						// true si feu; false si fumée
		qglviewer::Vec origin;				// origine du feu/fumée
		std::vector<Particule> particles;	// liste des particules
		int nbParticles;					// nombre de particules
		bool active;						// true si les particules se regénèrent
		bool inactivateReq;					// true si on a demandé l'arrêt du feu

		/**
		 * Initialise les particules
		 */
		void initParticle(Particule &p);

		/**
		 * Génère un nombre aléatoire entre min et max
		 */
		double alea(double min, double max);
};
#endif