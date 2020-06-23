#include "WorldState.h"
#include "ISA.h"
#include "ArchEvoGenUtil.h"
#include <stdlib.h>
#include <filesystem>
#include <iostream>
#include <fstream>
using namespace std;

void WorldState::new_tilde()
{
	int random_x = rand() % (size);
	int random_y = rand() % (size);

	if (get_cell(random_x, random_y) == nullptr)
	{
		CellState* the_cell = new CellState();
		if (tilde_mode == TILDE_MODE_RANDOM)
		{
			the_cell->make_random();
		}
		else
		{
			for (int i = 0; i < NUMBER_OF_GENES; i++)
			{
				the_cell->genes[i] = NPA_COP;
			}
			the_cell->energy = INITIAL_ENERGY;
		}
		place_cell(random_x, random_y, the_cell);
	}
}

WorldState::WorldState(int _size, int _pruning_rate, int _influx_rate, float _mutation_rate)
{

	begin = chrono::steady_clock::now();
	size = _size;
	pruning_rate = _pruning_rate;
	influx_rate = _influx_rate;
	iteration = 0;
	mutation_rate = _mutation_rate;
	create_world();
}

void WorldState::create_world()
{
	world = new CellState * *[size];
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
	if (auto_save && iteration % auto_save_rate == 0)
	{
		save_state(auto_save_file_name);
	}
	if (iteration % 100000 == 0)
	{
		auto stop = chrono::steady_clock::now();
		int minutes = chrono::duration_cast<chrono::minutes>(stop - begin).count();
		int seconds = chrono::duration_cast<chrono::seconds> (stop - begin).count() - minutes * 60;
		cout << iteration << " : " << minutes << "m " << seconds << "s" << endl;
		begin = stop;
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

void WorldState::save_state(string file_name)
{
	cout << "Saving..." << endl;
	if (!filesystem::exists(WORLD_DIR))
	{
		//Create world directory
		filesystem::create_directory(WORLD_DIR);
		cout << "Created world directory" << endl;
	}

	if (!filesystem::exists(WORLD_DIR + file_name))
	{
		filesystem::create_directory(WORLD_DIR + file_name);
		cout << "Created new world" << endl;
	}

	ofstream world_file;
	world_file.open(WORLD_DIR + file_name + "/world.txt");
	string general_info = "";
	general_info += to_string(size) + ",";
	general_info += to_string(iteration) + ",";
	general_info += to_string(pruning_rate) + ",";
	general_info += to_string(influx_rate) + ",";
	general_info += to_string(CellState::next_id) + ",";
	general_info += to_string(species_tracker.next_species_id);
	world_file << general_info << "\n";
	for (int x = 0; x < size; x++)
	{
		for (int y = 0; y < size; y++)
		{
			if (world[x][y] != nullptr)
			{
				string cell_string = "";
				cell_string += to_string(x) + ",";
				cell_string += to_string(y) + "|";
				cell_string += world[x][y]->get_save_string();
				//cout << world[x][y]->get_save_string() << endl;
				world[x][y]->load_from_string(world[x][y]->get_save_string());
				//cout << world[x][y]->get_save_string() << endl;
				world_file << cell_string << "\n";
			}
		}
	}
	world_file.close();

	species_tracker.save_state(WORLD_DIR + file_name + "/species");
}

void WorldState::load_state(string file_name)
{
	cout << "Loading" << endl;
	ifstream world_file;
	bool reading_general_info = true;
	world_file.open(WORLD_DIR + file_name + "/world.txt");
	string read_string;
	while (world_file >> read_string)
	{
		if (reading_general_info)
		{
			reading_general_info = false;
			size         = stoi(ArchEvoGenUtil::split_string(read_string, ",", 0));
			iteration    = stoi(ArchEvoGenUtil::split_string(read_string, ",", 1));
			pruning_rate = stoi(ArchEvoGenUtil::split_string(read_string, ",", 2));
			influx_rate  = stoi(ArchEvoGenUtil::split_string(read_string, ",", 3));
			CellState::next_id = stoi(ArchEvoGenUtil::split_string(read_string, ",", 4));
			species_tracker.next_species_id = stoi(ArchEvoGenUtil::split_string(read_string, ",", 5));
			create_world();
		}
		else
		{
			string coords = ArchEvoGenUtil::split_string(read_string, "|", 0);
			string cell_data = ArchEvoGenUtil::split_string(read_string, "|", 1);
			int x = stoi(ArchEvoGenUtil::split_string(coords, ",", 0));
			int y = stoi(ArchEvoGenUtil::split_string(coords, ",", 1));
			CellState* new_cell = new CellState;
			new_cell->load_from_string(cell_data);
			world[x][y] = new_cell;
		}
	}
	species_tracker.load_state(WORLD_DIR + file_name + "/species");
}

void WorldState::clear()
{
	iteration = 0;
	for (int x = 0; x < size; x++)
	{
		for (int y = 0; y < size; y++)
		{
			if (world[x][y] != nullptr)
			{
				delete world[x][y];
				world[x][y] = nullptr;
			}
		}
	}
}
