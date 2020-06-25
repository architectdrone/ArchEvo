#include "Species.h"
#include <type_traits>
#include <iostream>
#include "ISA.h"
#include "ArchEvoGenUtil.h"
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

string list_to_string(vector<vector<int>> list)
{
	string to_return = "";
	for (int i = 0; i < list.size(); i++)
	{
		int a = list[i][0];
		int b = list[i][1];
		to_return += to_string(a) + "-" + to_string(b) + ",";
	}
	return to_return;
}

vector<vector<int>> string_to_list(string list_string)
{
	vector<vector<int>> to_return;
	string just_read = "a";
	int i = 0;
	while (ArchEvoGenUtil::split_string(list_string, ",", i) != "")
	{
		just_read = ArchEvoGenUtil::split_string(list_string, ",", i);
		i++;
		int a = stoi(ArchEvoGenUtil::split_string(just_read, "-", 0));
		int b = stoi(ArchEvoGenUtil::split_string(just_read, "-", 1));
		vector<int> element = { a, b };
		to_return.push_back(element);
	}
	return to_return;
}

vector<int> condensed_list_to_uncondensed_list(vector<vector<int>> uncondensed_list)
{
	vector<int> to_return;
	for (int i = 0; i < uncondensed_list.size(); i++)
	{
		to_return = extend_and_increment(to_return, uncondensed_list[i][0], uncondensed_list[i][1]);
	}
	return to_return;
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
		//throw "Extinct species was born?";
		return;
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
		//throw "Extinct species ate?";
		return;
	}
	prey.set(prey.get(prey_species)+damage, prey_species);
}

void Species::register_being_eaten(int predator_species, int damage)
{
	if (extinct)
	{
		//throw "Extinct species was eaten?";
		return;
	}
	predators.set(predators.get(predator_species) + damage, predator_species);
}

void Species::register_dying(CellState* dying_cell, int date)
{
	if (extinct)
	{
		//throw "Extinct species died?";
		return;
	}
	virilities.set(virilities.get(dying_cell->virility) + 1, dying_cell->virility);
	ages.set(ages.get(dying_cell->age) + 1, dying_cell->age);
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
		children.set(1, id);
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
		return prey.condense();
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
		return predators.condense();
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
		return ages.condense();
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
		return virilities.condense();
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
		return children.simple_condense();
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

string Species::get_save_string()
{
	string to_return = "";
	string general_info = "";
	general_info += to_string(number_alive)    + ","; //0
	general_info += to_string(total_alive)     + ","; //1
	general_info += to_string(peak_alive)      + ","; //2
	general_info += to_string(extinction_date) + ","; //3
	general_info += to_string(id)              + ","; //4
	general_info += to_string(parent_id)       + ","; //5
	general_info += readable_id                + ","; //6
	general_info += to_string(arrival_date);          //7

	to_return += general_info + ";";

	string genome_string = "";
	for (int i = 0; i < NUMBER_OF_GENES; i++)
	{
		genome_string.append(to_string(genome[i]));
		genome_string.append(",");
	}

	to_return += genome_string + ";";

	to_return += list_to_string(all_prey()) + ";";
	to_return += list_to_string(all_predators()) + ";";
	to_return += list_to_string(all_ages()) + ";";
	to_return += list_to_string(all_virilities()) + ";";
	string children_string = "";
	vector<int> the_children = all_children();
	for (int i = 0; i < the_children.size(); i++)
	{
		children_string += to_string(the_children[i]) + ",";
	}
	to_return += children_string + ";";
	return to_return;
}

void Species::load_from_string(string read_string, bool is_extinct)
{
	string general_info      = ArchEvoGenUtil::split_string(read_string, ";", 0);
	string genome_string     = ArchEvoGenUtil::split_string(read_string, ";", 1);
	string prey_string       = ArchEvoGenUtil::split_string(read_string, ";", 2);
	string predators_string  = ArchEvoGenUtil::split_string(read_string, ";", 3);
	string ages_string       = ArchEvoGenUtil::split_string(read_string, ";", 4);
	string virilities_string = ArchEvoGenUtil::split_string(read_string, ";", 5);
	string children_string   = ArchEvoGenUtil::split_string(read_string, ";", 6);

	number_alive    = stoi(ArchEvoGenUtil::split_string(general_info, ",", 0));
	total_alive     = stoi(ArchEvoGenUtil::split_string(general_info, ",", 1));
	peak_alive      = stoi(ArchEvoGenUtil::split_string(general_info, ",", 2));
	extinction_date = stoi(ArchEvoGenUtil::split_string(general_info, ",", 3));
	id              = stoi(ArchEvoGenUtil::split_string(general_info, ",", 4));
	parent_id       = stoi(ArchEvoGenUtil::split_string(general_info, ",", 5));
	readable_id     =      ArchEvoGenUtil::split_string(general_info, ",", 6) ;
	arrival_date    = stoi(ArchEvoGenUtil::split_string(general_info, ",", 7));

	
	for (int i = 0; i < NUMBER_OF_GENES; i++)
	{
		genome[i] = stoi(ArchEvoGenUtil::split_string(genome_string, ",", i));
	}

	vector<vector<int>> read_prey       = string_to_list(prey_string);
	vector<vector<int>> read_predators  = string_to_list(predators_string);
	vector<vector<int>> read_ages       = string_to_list(ages_string);
	vector<vector<int>> read_virilities = string_to_list(virilities_string);

	vector<int> read_children;
	
	string just_read;
	int i = 0;
	while (ArchEvoGenUtil::split_string(children_string, ",", i) != "")
	{
		just_read = ArchEvoGenUtil::split_string(children_string, ",", i);
		i++;
		int a = stoi(just_read);
		read_children.push_back(a);
	}
	extinct = is_extinct;
	if (!is_extinct)
	{
		prey.uncondense(read_prey);
		predators.uncondense(read_predators);
		ages.uncondense(read_ages);
		virilities.uncondense(read_virilities);
		for (int i = 0; i < read_children.size(); i++)
		{
			children.set(1, read_children[i]);
		}
	}
	else
	{
		extinct_prey = read_prey;
		extinct_predators = read_predators;
		extinct_ages = read_ages;
		extinct_virilities = read_virilities;
		extinct_children = read_children;
	}
}
