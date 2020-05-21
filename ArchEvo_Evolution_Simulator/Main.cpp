#include "CellState.h"
#include "ISA.h"
#include <iostream>
using namespace std;
int main()
{
	const int size = 25;
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
		int random_x = rand() % (size);
		int random_y = rand() % (size);
		world[random_x][random_y] = new CellState();
		world[random_x][random_y]->make_random();
		iterations++;		
		
		bool print_time = iterations % 10000 == 0;

		int NVO_count = 0;
		int valid_organism_count = 0;

		for (int x = 0; x < size; x++)
		{
			for (int y = 0; y < size; y++)
			{
				ISA::execute(x, y, world, size);
				if (world[x][y] != nullptr && world[x][y]->lineage_length != 0)
				{
					ISA::print_info(world[x][y]);
				}
			}
		}
	}
}