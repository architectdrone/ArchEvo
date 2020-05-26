#pragma once
#include "CellState.h"
#include "SpeciesTracker.h"
#define WORLD_DIR "C:/ArchEvo/"

class WorldState
{
private:
	int size;
	int iteration;
	int pruning_rate;
	
	void new_tilde();

public:
	WorldState(int _size, int _pruning_rate, int _influx_rate);
	void create_world();
	~WorldState();
	CellState*** world;

	void update();
	CellState* get_cell(int x, int y);
	void place_cell(int x, int y, CellState* cell);
	void delete_cell(int x, int y);
	void swap_cells(int x_1, int y_1, int x_2, int y_2);

	int get_size();
	int get_iteration();

	int influx_rate;
	SpeciesTracker species_tracker;

	bool auto_save;
	int auto_save_rate;
	string auto_save_file_name;


	void save_state(string file_name);
	void load_state(string file_name);
};