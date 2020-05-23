#pragma once
#include <vector>
#include "Species.h"
class SpeciesTracker
{
private:
	vector<Species*> species_list;
	vector<Species*> extinct_species_list;
	int next_species_id = 0;

public:
	Species* get_species(int species_id);
	vector<Species*> get_all_species();
	int number_of_living_species();
	int number_of_extinct_species();
	void prune_extinct_species();
	bool create_new_species_if_needed(CellState* parent_cell, CellState* child_cell, int date);
	void extinction_if_needed(CellState* dead_cell, int date);
	void print_all_species();
};