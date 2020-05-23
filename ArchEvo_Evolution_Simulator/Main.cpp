#include "CellState.h"
#include "ISA.h"
#include "Viewer.h"
#include "libtcod.hpp"
#include <iostream>
using namespace std;

int main()
{
	const int size = 20;
	CellState*** world = new CellState**[size];
	for (int x = 0; x < size; x++)
	{
		world[x] = new CellState*[size];
		for (int y = 0; y < size; y++)
		{
			world[x][y] = nullptr;
		}
	}

	
	Viewer::init(size);
	Viewer::draw_mode = DRAW_SPECIES;
	//ISA::init();
	//Main Loop
	int iterations = 0;
	const int print_rate = 10000;
	const int prune_rate = 10000;
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

		if (iterations % prune_rate == 0)
		{
			cout << "Pruning..." << endl;
			ISA::prune_extinct_species();
		}

		int organism_count = 0;
		int valid_organism_count = 0;

		CellState* longest_lineage = nullptr;

		if (print_time)
		{
			ISA::print_all_species();
		}

		for (int x = 0; x < size; x++)
		{
			for (int y = 0; y < size; y++)
			{
				ISA::execute(x, y, world, size, iterations);
				if (world[x][y] != nullptr)
				{
					organism_count++;
					if (world[x][y]->lineage_length > 1)
					{
						valid_organism_count++;
						if (print_time)
						{
							//cout << "------------------------------------------" << endl;
							//ISA::print_info(world[x][y]);
							//cout << "------------------------------------------" << endl;
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
		Viewer::draw(world);
		if (print_time)
		{
			cout << "--------------------------------------------------------" << endl;

			ISA::print_info(longest_lineage);
			ISA::print_genome(longest_lineage);
			int num;
			cin >> num;
		}
		cout << "Iteration " << iterations << " S: " << ISA::number_of_living_species() << " E: " << ISA::number_of_extinct_species() << endl;
	}
	
}