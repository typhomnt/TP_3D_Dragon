/**
 * @brief Classe permettant de gérer une particule
 * @author Julien Daval - Maxime Garcia - Omid Ghorreshi
 */

#ifndef DEF_PARTICULE
#define DEF_PARTICULE

class Particule {
	public:
        // Accesseur + mutateur pour l'attribut active
		bool isActive();
		void setActive(bool active);

        // Accesseur + mutateurs pour l'attribut life
		double getLife();
		void setLife(double life);
		void incrLife(double incr);

        // Accesseur + mutateur pour l'attribut velDis
		double getVelDis();
		void setVelDis(double velDis);

        // Accesseur + mutateurs pour l'attribut color
		qglviewer::Vec getColor();
		void setColor(qglviewer::Vec color);
		void incrColor(qglviewer::Vec incr);

        // Accesseur + mutateurs pour l'attribut pos
		qglviewer::Vec getPos();
		void setPos(qglviewer::Vec pos);
		void incrPos(qglviewer::Vec incr);

        // Accesseur + mutateurs pour l'attribut movVec
		qglviewer::Vec getMovVec();
		void setMovVec(qglviewer::Vec movVec);
		void incrMovVec(qglviewer::Vec incr);

        // Accesseur + mutateur pour l'attribut gravity
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
