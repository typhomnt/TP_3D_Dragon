/**
 * @brief Classe permettant de gérer l'herbe
 * @author Julien Daval - Maxime Garcia - Omid Ghorreshi
 */

#ifndef DEF_GRASS
#define DEF_GRASS

#include <vector>
#include "renderable.h"
#include "particule.h"


class Grass : public Renderable {
	public:
		Grass(double size, int nbParticles, int iter);
		void init(Viewer &v);
		void draw();

		void setRadius(double radius);
		void setZg(double zg);

	private:
		double size;						// taille
		int iter;							// nb d'itérations pour générer les brins
		std::vector<Particule> base; 		// particules initiales
		std::vector<Particule> elements;	// particules à dessiner

		double radius;						// Rayon des particules de base
		double radiusStep;					// Diminution du rayon à chaque étape
		double zg;							// Composante selon z du poids


        /**
         * @brief Initialise les particules
         */
		void initParticles();

        /**
         * @brief Tire un nombre aléatoire entre min et max
         */
		double alea(double min, double max);
};
#endif
