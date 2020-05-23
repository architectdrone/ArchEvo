#include "CellState.h"
#include "ISA.h"
#include "Viewer.h"
#include "libtcod.hpp"
#include <iostream>

using namespace std;

int main()
{
	WorldState* world = new WorldState(20, 10000, 3);
	Viewer::init(20);
	Viewer::draw_mode = DRAW_SPECIES;
	int i = 0;
	while (i < 100)
	{
		i++;
		world->update();
		Viewer::draw(world);
		cout << "Iteration: " << world->get_iteration() << " S: " << world->species_tracker.number_of_living_species() << " E: " << world->species_tracker.number_of_extinct_species() << endl;
	}
	delete world;	
}