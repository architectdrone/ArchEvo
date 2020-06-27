#pragma once
#include <vector>
#include "Species.h"
class SpeciesTracker
{
private:
	void sort_species_list();
	void sort_extinct_species_list();

	vector<Species*> species_list;
	vector<Species*> extinct_species_list;
	

public:
	~SpeciesTracker();
	Species* get_species(int species_id, bool ensure_sorted = true);
	Species* get_species_rank(int rank);
	Species* get_species_rank_extinct(int rank);
	vector<Species*> get_all_species();
	int number_of_living_species();
	int number_of_extinct_species();
	void prune_extinct_species();
	bool create_new_species_if_needed(CellState* parent_cell, CellState* child_cell, int date);
	void extinction_if_needed(CellState* dead_cell, int date);
	void print_all_species();
	void ensure_sorted();

	void clear();
	
	void save_species_vector(string filename, vector<Species*> input);
	void load_species_vector(string filename, bool extinct);

	void save_state(string filename);
	void load_state(string filename);
	int next_species_id = 0;

	bool store_extinct_species = true;
};
