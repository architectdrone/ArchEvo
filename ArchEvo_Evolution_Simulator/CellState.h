#pragma once
#include <string>
#define NUMBER_OF_GENES 32
#define TILDE_LOGO 0
#define INITIAL_ENERGY 64
using namespace std;
class CellState
{
private:
	
public:
	void make_child(CellState parent, float mutation_rate);
	void make_random();

	//Genetic Information
	int ip = 0;
	int genes[NUMBER_OF_GENES];

	//Physical Registers
	int energy = 0;
	int logo   = 0;
	int guess  = 0;
	int reg_a  = 0;
	int reg_b  = 0;
	int reg_c  = 0;
	int reg_d  = 0;
	int iploc  = 0;

	//Data
	int age = 0;
	int lineage_length = 0;
	int species_id = 0;
	int virility = 0;

	int id = 0;
	int last_iteration = -1;

	string get_save_string();
	void load_from_string(string to_load);

	static int next_id;
	bool infinite_energy = false;
};