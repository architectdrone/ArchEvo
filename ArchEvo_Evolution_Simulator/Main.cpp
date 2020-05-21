#include "CellState.h"
#include "ISA.h"
#include <iostream>
using namespace std;
int main()
{
	const int size = 2;
	CellState*** world = new CellState**[size];
	for (int x = 0; x < size; x++)
	{
		world[x] = new CellState*[size];
		for (int y = 0; y < size; y++)
		{
			world[x][y] = nullptr;
		}
	}

	CellState* new_cell_state = new CellState();
	new_cell_state->genes[0] = ISA::create_instruction(DIV_COP); //Should jump to ip=5
	new_cell_state->genes[1] = ISA::create_instruction(NPA_COP);
	new_cell_state->genes[2] = ISA::create_instruction(NPB_COP);
	new_cell_state->genes[3] = ISA::create_instruction(IGN_COP);
	new_cell_state->genes[4] = ISA::create_instruction(IGN_COP);
	new_cell_state->genes[5] = ISA::create_instruction(NPA_COP);
	new_cell_state->genes[6] = ISA::create_instruction(NPB_COP);
	new_cell_state->genes[7] = ISA::create_instruction(IGN_COP);
	new_cell_state->genes[8] = ISA::create_instruction(IGN_COP);
	new_cell_state->genes[9] = ISA::create_instruction(IGN_COP);
	new_cell_state->energy = 128;

	world[1][1] = new_cell_state;

	ISA::print_info(world[1][1]);
	ISA::print_genome(world[1][1]);

	for (int i = 0; i < NUMBER_OF_GENES * 2; i++)
	{
		ISA::execute(1, 1, world, size);
	}
}