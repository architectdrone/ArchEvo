#pragma once
#include <vector>
#include <string>
#include "CellState.h"
#include "FastSpeciesContainer.h"

#define TOLERABLE_DIFFERENCE_AMOUNT 0.1 //If a genome differs by greater than this percent from the species genome, they are different species.

using namespace std;
class Species
{	
private:
	//For predators and prey, each index is an ID. For example, if prey[4] = 5, that means organism with ID 4 was eaten 5 times.
	FastSpeciesContainer prey;
	FastSpeciesContainer predators;

	//For children, ages, and virilities
	FastSpeciesContainer children;
	FastSpeciesContainer ages;
	FastSpeciesContainer virilities;
	
	//Extinction caches - this reduces the size of the species in memory as soon as it goes extinct.
	void create_extinction_cache();
	bool extinct = false;
	vector<vector<int>> extinct_prey;
	vector<vector<int>> extinct_predators;
	vector<int> extinct_children;
	vector<vector<int>> extinct_ages;
	vector<vector<int>> extinct_virilities;

	int number_alive = 0; //Number alive now
	int total_alive = 0; //Total of this species that has ever lived.
	int peak_alive = 0; //Most alive at one point.
	int extinction_date = -1; //Date that the species went extinct (if ever)
public:
	void register_birth();
	void register_eating(int prey_species, int damage);
	void register_being_eaten(int predator_species, int damage);
	void register_dying(CellState* dying_cell, int date);
	void register_child_species(int id);

	bool tolerable_difference(CellState* cell); //Returns false if a new species should be created.

	vector<vector<int>> all_prey(); //Returns list of prey animals. The first element is the ID, the second is the number of times it was eaten.
	vector<vector<int>> all_predators(); //See above.
	vector<vector<int>> all_ages();
	vector<vector<int>> all_virilities();
	vector<int> all_children(); //Get list of children
	float average_age(); //Get average age of species at death.
	float average_virility(); //Get average virility at death
	

	int get_alive();
	int get_total_alive();
	int get_peak_alive();
	int get_extinction_date();

	int id;
	int parent_id;
	string readable_id;
	int genome[NUMBER_OF_GENES];
	int arrival_date;

	string get_save_string();
	void load_from_string(string read_string, bool extinct);
};