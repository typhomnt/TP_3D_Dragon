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

	private:
		double size;						// taille
		int iter;							// nb d'itérations pour générer les brins
		std::vector<Particule> base; 		// particules initiales
		std::vector<Particule> elements;	// particules à dessiner

		void initParticles();

		double alea(double min, double max);

		void drawBasePlane(double size);
};
#endif
