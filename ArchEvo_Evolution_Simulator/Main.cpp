#include "CellState.h"
#include "ISA.h"
#include "Viewer.h"
#include "libtcod.hpp"
#include <iostream>

using namespace std;

int main()
{
	int size = 50;
	WorldState* world = new WorldState(size, 10000, 3);
	Viewer::init();
	Viewer::draw_mode = DRAW_SPECIES;
	while (true)
	{
		world->update();
		Viewer::draw(world);
		cout << "Iteration: " << world->get_iteration() << " S: " << world->species_tracker.number_of_living_species() << " E: " << world->species_tracker.number_of_extinct_species() << endl;
	}
	delete world;	
}