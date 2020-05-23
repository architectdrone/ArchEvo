#pragma once
#include "CellState.h"
class WorldState
{
private:
	int size;
	int iteration;
	int pruning_rate;
	
	void new_tilde();

public:
	WorldState(int _size, int _pruning_rate, int _influx_rate);
	CellState*** world;

	void update();

	int get_size();
	int get_iteration();

	int influx_rate;
};