/**
 * @brief Classe permettant de gérer le feu ou la fumée du dragon
 * @author Julien Daval - Maxime Garcia - Omid Ghorreshi
 */

#ifndef DEF_FIRESMOKE
#define DEF_FIRESMOKE

#include <vector>
#include "renderable.h"
#include "particule.h"


class FireSmoke : public Renderable {
	public:
        FireSmoke(bool firesmoke, qglviewer::Vec origin, int nbParticles = 1000, bool dust=false);
		void init(Viewer &v);
		void draw();
		void animate();

        // Accesseur + mutateur pour l'attribut origin
		qglviewer::Vec getOrigin();
		void setOrigin(qglviewer::Vec origin);

        /**
         * @brief Renvoie true si le feu/fumée est actif; false sinon
         */
		bool isActive();

        /**
         * @brief Active le dispositif
         */
		void activate();

        /**
         * @brief Désactive le dispositif
         */
		void inactivate();

	private:
		bool firesmoke;						// true si feu; false si fumée
		qglviewer::Vec origin;				// origine du feu/fumée
		std::vector<Particule> particles;	// liste des particules
		int nbParticles;					// nombre de particules
		bool active;						// true si les particules se regénèrent
		bool inactivateReq;					// true si on a demandé l'arrêt du feu
        bool dust;                          // true si on simule la poussière


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
