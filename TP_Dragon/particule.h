#ifndef DEF_PARTICULE
#define DEF_PARTICULE


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
		void incrColor(qglviewer::Vec incr);

		qglviewer::Vec getPos();
		void setPos(qglviewer::Vec pos);
		void incrPos(qglviewer::Vec incr);

		qglviewer::Vec getMovVec();
		void setMovVec(qglviewer::Vec movVec);
		void incrMovVec(qglviewer::Vec incr);

		qglviewer::Vec getGravity();
		void setGravity(qglviewer::Vec gravity);


	private:
		bool active;			// true si la particule est active, false sinon
		double life;			// temps de vie de la particule
		double velDis;			// vitesse de disparition
		qglviewer::Vec color;	// couleur de la particule
		qglviewer::Vec pos;		// position de la particule
		qglviewer::Vec movVec;	// vecteur de mouvement
		qglviewer::Vec gravity;	// gravité
};

#endif