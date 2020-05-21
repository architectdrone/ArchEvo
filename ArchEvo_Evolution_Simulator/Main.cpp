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
	for (int i = 0; i < NUMBER_OF_GENES; i++)
	{
		new_cell_state->genes[i] = ISA::create_instruction(ATK_COP);
	}
	new_cell_state->energy = 128;
	new_cell_state->guess = 0b11111110;
	world[1][1] = new_cell_state;

	
	CellState* victim = new CellState;
	for (int i = 0; i < NUMBER_OF_GENES; i++)
	{
		victim->genes[i] = ISA::create_instruction(IGN_COP);
	}
	victim->energy = 128;
	victim->logo = 0b11111111;
	world[0][0] = victim;

	for (int i = 0; i < 10; i++)
	{
		cout << "ATTACKER--------------------------" << endl;
		ISA::print_info(world[1][1]);
		cout << "----------------------------------" << endl;
		cout << "DEFENDER--------------------------" << endl;
		ISA::print_info(world[0][0]);
		cout << "----------------------------------" << endl;
		ISA::execute(1, 1, world, size);
	}
}