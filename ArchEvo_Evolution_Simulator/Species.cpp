#include "Species.h"
#include <type_traits>
#include <iostream>
#include "ISA.h"
using namespace std;

vector<int> extend_and_increment(vector<int> input, int index, int amount = 1)
{
	if (index >= input.size())
	{
		input.resize(index+1);
		input[index] = 0;
	}
	input[index] += amount;
	return input;
}

vector<vector<int>> condense_list(vector<int> list)
{
	vector<vector<int>> condensed_list;
	for (int i = 0; i < list.size(); i++)
	{
		if (list[i] != 0)
		{
			vector<int> new_entry;
			new_entry.resize(2);
			new_entry[0] = i;
			new_entry[1] = list[i];
			condensed_list.push_back(new_entry);
		}
	}
	return condensed_list;
}

float list_average(vector<vector<int>> condensed)
{
	int total = 0;
	int sum = 0;
	for (int i = 0; i < condensed.size(); i++)
	{
		total += condensed[i][1];
		sum += condensed[i][1] * condensed[i][0];
	}
	return (float)sum / (float)total;
}

void Species::create_extinction_cache()
{
	extinct_ages = all_ages();
	extinct_virilities = all_virilities();
	extinct_prey = all_prey();
	extinct_predators = all_predators();
	extinct_children = all_children();
	
	prey.clear();
	predators.clear();
	children.clear();
	ages.clear();
	virilities.clear();

	extinct = true;
}

void Species::register_birth()
{
	if (extinct)
	{
		throw "Extinct species was born?";
	}
	number_alive++;
	total_alive++;
	if (number_alive > peak_alive)
	{
		peak_alive = number_alive;
	}
}

void Species::register_eating(int prey_species, int damage)
{
	if (extinct)
	{
		throw "Extinct species ate?";
	}
	prey = extend_and_increment(prey, prey_species, damage);
}

void Species::register_being_eaten(int predator_species, int damage)
{
	if (extinct)
	{
		throw "Extinct species was eaten?";
	}
	predators = extend_and_increment(predators, predator_species, damage);
}

void Species::register_dying(CellState* dying_cell, int date)
{
	if (extinct)
	{
		throw "Extinct species died?";
	}
	virilities = extend_and_increment(virilities, dying_cell->virility);
	ages = extend_and_increment(ages, dying_cell->age);
	number_alive--;

	if (number_alive == 0)
	{
		//Extinction
		extinction_date = date;
	}
	
}

void Species::register_child_species(int id)
{
	if (!extinct)
	{
		children = extend_and_increment(children, id);
	}
	else
	{
		extinct_children.push_back(id);
	}
	
}

bool Species::tolerable_difference(CellState* cell)
{
	int differences = 0;
	for (int gene = 0; gene < NUMBER_OF_GENES; gene++)
	{
		for (int bit = 0; bit < 11; bit++)
		{
			int species_bit = (genome[gene] >> (bit)) & 0b1;
			int unknown_bit = (cell->genes[gene] >> (bit)) & 0b1;
			if (species_bit != unknown_bit)
			{
				differences++;
			}
		}
	}
	float difference_ratio = (float)differences / (float)(NUMBER_OF_GENES * 11);
	return difference_ratio < TOLERABLE_DIFFERENCE_AMOUNT;
}

vector<vector<int>> Species::all_prey()
{
	if (!extinct)
	{
		return condense_list(prey);
	}
	else
	{
		return extinct_prey;
	}
}

vector<vector<int>> Species::all_predators()
{
	if (!extinct)
	{
		return condense_list(predators);
	}
	else
	{
		return extinct_predators;
	}
}

vector<vector<int>> Species::all_ages()
{
	if (!extinct)
	{
		return condense_list(ages);
	}
	else
	{
		return extinct_ages;
	}
}

vector<vector<int>> Species::all_virilities()
{
	if (!extinct)
	{
		return condense_list(virilities);
	}
	else
	{
		return extinct_virilities;
	}
	
}

float Species::average_age()
{
	return list_average(all_ages());
}

float Species::average_virility()
{
	return list_average(all_virilities());
}

vector<int> Species::all_children()
{
	if (!extinct)
	{
		vector<int> to_return;
		for (int i = 0; i < children.size(); i++)
		{
			if (children[i] != 0)
			{
				to_return.push_back(i);
			}
		}
		return to_return;
	}
	else
	{
		return extinct_children;
	}
	
}

int Species::get_alive()
{
	return number_alive;
}

int Species::get_total_alive()
{
	return total_alive;
}

int Species::get_peak_alive()
{
	return peak_alive;
}

int Species::get_extinction_date()
{
	return extinction_date;
}