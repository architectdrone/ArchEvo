#include "CellState.h"
#include "ISA.h"
#include "Viewer.h"
#include "libtcod.hpp"
#include <iostream>

using namespace std;

int main()
{
	int size = 20;
	WorldState* world = new WorldState(size, 10000, 1);
	Viewer::init();
	Viewer::draw_mode = DRAW_SPECIES;
	int count = 0;
	while (true)
	{
		count++;
		if ((Viewer::speed != SPEED_PAUSED) && (Viewer::speed != SPEED_SLOW || count % 100 == 0))
		{
			world->update();
		}
		Viewer::draw(world);
		//cout << "Iteration: " << world->get_iteration() << " S: " << world->species_tracker.number_of_living_species() << " E: " << world->species_tracker.number_of_extinct_species() << endl;
	}
	delete world;	
}