#include "CellState.h"
#include "ISA.h"
#include "Viewer.h"
#include "libtcod.hpp"
#include "GenebankGenerator.h"
#include "FastSpeciesContainer.h"
#include <unordered_map>
#include <iostream>

using namespace std;

int main()
{
	cout << "\"in principio creavit Deus caelum et terram\"" << endl;
	int size = 20;
	WorldState* world = new WorldState(size, 10000, 1, 0.001f);
	
	/*
	CellState* test_cell = new CellState;
	test_cell->energy = 256;
	test_cell->iploc = 0;
	test_cell->genes[0] = ISA::create_instruction(SET_ROP, C_REG, D_REG);
	test_cell->genes[1] = ISA::create_instruction(NPB_COP);
	test_cell->genes[2] = ISA::create_instruction(INC_ROP, B_REG);
	test_cell->genes[3] = ISA::create_instruction(JPC_COP, C_REG, true);
	test_cell->genes[4] = ISA::create_instruction(NPB_COP);
	test_cell->genes[5] = ISA::create_instruction(INC_ROP, C_REG);
	for (int i = 6; i < NUMBER_OF_GENES; i++)
	{
		test_cell->genes[i] = ISA::create_instruction(IGN_COP);
	}
	world->place_cell(10, 10, test_cell);
	*/

	Viewer::init();
	Viewer::draw_mode = DRAW_SPECIES;
	int count = 0;
	bool has_saved = false;
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