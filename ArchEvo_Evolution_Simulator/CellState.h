#pragma once
#define NUMBER_OF_GENES = 32;
#define MUTATION_RATE = 0.01; //Chance of a single bit in a single cell flipping.
#define NVO_LOGO = 255;
#define INITIAL_ENERGY = 64;
class CellState
{
public:
	void make_child(CellState parent);
	void make_random();

	int energy = 0;
	int logo   = 0;
	int guess = 0;
	int reg_a  = 0;
	int reg_b  = 0;
	int reg_c  = 0;
	int reg_d  = 0;
	int iploc  = 0;

	int ip = 0;
	int genes[NUMBER_OF_GENES];
};