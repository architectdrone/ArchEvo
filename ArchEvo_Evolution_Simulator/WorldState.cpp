#include "WorldState.h"
#include "ISA.h"
#include "ArchEvoGenUtil.h"
#include <stdlib.h>

void WorldState::new_tilde()
{
	int random_x = rand() % (size);
	int random_y = rand() % (size);

	if (get_cell(random_x, random_y) == nullptr)
	{
		CellState* the_cell = new CellState();
		the_cell->make_random();
		place_cell(random_x, random_y, the_cell);
	}
}

WorldState::WorldState(int _size, int _pruning_rate, int _influx_rate)
{
	size = _size;
	pruning_rate = _pruning_rate;
	influx_rate = _influx_rate;
	iteration = 0;
	world = new CellState **[size];
	for (int x = 0; x < size; x++)
	{
		world[x] = new CellState * [size];
		for (int y = 0; y < size; y++)
		{
			world[x][y] = nullptr;
		}
	}
}

WorldState::~WorldState()
{
	for (int x = 0; x < size; x++)
	{
		for (int y = 0; y < size; y++)
		{
			delete world[x][y];
		}
		delete world[x];
	}
	delete world;
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
		species_tracker.prune_extinct_species();
	}


	for (int x = 0; x < size; x++)
	{
		for (int y = 0; y < size; y++)
		{
			ISA::execute(x, y, this);
		}
	}
}

CellState* WorldState::get_cell(int x, int y)
{
	int true_x = ArchEvoGenUtil::true_mod(x, size);
	int true_y = ArchEvoGenUtil::true_mod(y, size);
	return world[true_x][true_y];
}

void WorldState::place_cell(int x, int y, CellState* cell)
{
	int true_x = ArchEvoGenUtil::true_mod(x, size);
	int true_y = ArchEvoGenUtil::true_mod(y, size);
	if (get_cell(true_x, true_y) != nullptr)
	{
		throw "Attempting to overwrite a filled cell, this will cause memory leaks!";
	}
	world[true_x][true_y] = cell;
}

void WorldState::delete_cell(int x, int y)
{
	int true_x = ArchEvoGenUtil::true_mod(x, size);
	int true_y = ArchEvoGenUtil::true_mod(y, size);
	if (world[true_x][true_y] != nullptr)
	{
		delete world[true_x][true_y];
		world[true_x][true_y] = nullptr;
	}
}

void WorldState::swap_cells(int x_1, int y_1, int x_2, int y_2)
{
	int real_x_1 = ArchEvoGenUtil::true_mod(x_1, size);
	int real_x_2 = ArchEvoGenUtil::true_mod(x_2, size);
	int real_y_1 = ArchEvoGenUtil::true_mod(y_1, size);
	int real_y_2 = ArchEvoGenUtil::true_mod(y_2, size);
	CellState* temp = world[real_x_1][real_y_1];
	world[real_x_1][real_y_1] = world[real_x_2][real_y_2];
	world[real_x_2][real_y_2] = temp;
}

int WorldState::get_size()
{
	return size;
}

int WorldState::get_iteration()
{
	return iteration;
}
