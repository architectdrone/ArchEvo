#include "WorldState.h"
#include "ISA.h"
#include <stdlib.h>

void WorldState::new_tilde()
{
	int random_x = rand() % (size);
	int random_y = rand() % (size);

	if (world[random_x][random_y] == nullptr)
	{
		world[random_x][random_y] = new CellState();
		world[random_x][random_y]->make_random();
	}
}

WorldState::WorldState(int _size, int _pruning_rate, int _influx_rate)
{
	size = _size;
	pruning_rate = _pruning_rate;
	influx_rate = _influx_rate;
	CellState*** world = new CellState **[size];
	for (int x = 0; x < size; x++)
	{
		world[x] = new CellState * [size];
		for (int y = 0; y < size; y++)
		{
			world[x][y] = nullptr;
		}
	}
}

void WorldState::update()
{
	//Add new tildes
	for (int i = 0; i < influx_rate; i++)
	{
		new_tilde();
	}

	iteration++;

	if (iteration % pruning_rate == 0)
	{
		ISA::prune_extinct_species();
	}


	for (int x = 0; x < size; x++)
	{
		for (int y = 0; y < size; y++)
		{
			ISA::execute(x, y, world, size, iteration);
		}
	}
}

int WorldState::get_size()
{
	return size;
}

int WorldState::get_iteration()
{
	return iteration;
}
