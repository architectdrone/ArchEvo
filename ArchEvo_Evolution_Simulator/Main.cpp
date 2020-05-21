#include "CellState.h"
#include "ISA.h"
#include <iostream>
using namespace std;
int main()
{
	const int size = 100;
	CellState*** world = new CellState**[size];
	for (int x = 0; x < size; x++)
	{
		world[x] = new CellState*[size];
		for (int y = 0; y < size; y++)
		{
			world[x][y] = nullptr;
		}
	}

	//Main Loop
	int iterations = 0;
	while (true)
	{
		cout << "Iteration " << iterations << endl;
		iterations++;
		//Create NVO
		int random_x = rand() % (size);
		int random_y = rand() % (size);
		world[random_x][random_y] = new CellState();
		world[random_x][random_y]->make_random();
		ISA::print_genome(world[random_x][random_y]);
		for (int x = 0; x < size; x++)
		{
			for (int y = 0; y < size; y++)
			{
				ISA::execute(x, y, world, size);
			}
		}
	}
}