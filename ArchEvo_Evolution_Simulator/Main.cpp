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
	const int print_rate = 100000;
	while (true)
	{
		
		int random_x = rand() % (size);
		int random_y = rand() % (size);

		if (world[random_x][random_y] == nullptr)
		{
			world[random_x][random_y] = new CellState();
			world[random_x][random_y]->make_random();
		}

		
		iterations++;		
		
		bool print_time = iterations % print_rate == 0;

		int organism_count = 0;
		int valid_organism_count = 0;

		CellState* longest_lineage = nullptr;

		for (int x = 0; x < size; x++)
		{
			for (int y = 0; y < size; y++)
			{
				ISA::execute(x, y, world, size);
				if (world[x][y] != nullptr)
				{
					organism_count++;
					if (world[x][y]->lineage_length > 1)
					{
						valid_organism_count++;
						if (print_time)
						{
							cout << "------------------------------------------" << endl;
							ISA::print_info(world[x][y]);
							cout << "------------------------------------------" << endl;
							if (longest_lineage == nullptr)
							{
								longest_lineage = world[x][y];
							}
							else if (longest_lineage->lineage_length < world[x][y]->lineage_length)
							{
								longest_lineage = world[x][y];
							}
						}
					}
				}
			}
		}
		if (print_time)
		{
			cout << "BEST GENOME: " << endl;
			ISA::print_genome(longest_lineage);
			int num;
			cin >> num;
		}
		cout << "Iteration " << iterations << " O: " << organism_count << " V: " << valid_organism_count << endl;
	}
	
}