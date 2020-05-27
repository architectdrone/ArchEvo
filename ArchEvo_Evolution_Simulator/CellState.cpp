#include "CellState.h"
#include <string>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <cmath>
#include <iostream>
#include <bitset>
#include <sstream>
#include "ArchEvoGenUtil.h"

int CellState::next_id = 0;
using namespace std;
void CellState::make_random()
{
	for (int gene = 0; gene < NUMBER_OF_GENES; gene++)
	{
		for (int bit = 0; bit < 11; bit++)
		{
			float random_num = ((double)rand() / (RAND_MAX));
			genes[gene] = genes[gene] << 1;
			if (random_num > 0.5)
			{
				genes[gene] += 1;
			}
		}
	}
	energy = INITIAL_ENERGY;
	logo = TILDE_LOGO;
	lineage_length = 0;
	id = next_id++;
}

string CellState::get_save_string()
{
	string genome_string = "";
	for (int i = 0; i < NUMBER_OF_GENES; i++)
	{
		genome_string.append(to_string(genes[i]));
		genome_string.append(",");
	}

	string to_return = "";
	to_return += to_string(energy) + ",";
	to_return += to_string(logo)   + ",";
	to_return += to_string(guess)  + ",";
	to_return += to_string(reg_a)  + ",";
	to_return += to_string(reg_b)  + ",";
	to_return += to_string(reg_c)  + ",";
	to_return += to_string(reg_d)  + ",";
	to_return += to_string(iploc)  + ";";

	to_return += to_string(age)            + ",";
	to_return += to_string(lineage_length) + ",";
	to_return += to_string(species_id)     + ",";
	to_return += to_string(virility)       + ",";
	to_return += to_string(id)             + ";";

	to_return += to_string(ip) + ";";

	to_return += genome_string + ";";
	return to_return;
}


void CellState::load_from_string(string to_load)
{
	string regs = ArchEvoGenUtil::split_string(to_load, ";", 0);
	string data = ArchEvoGenUtil::split_string(to_load, ";", 1);
	string ip_str = ArchEvoGenUtil::split_string(to_load, ";", 2);
	string genome = ArchEvoGenUtil::split_string(to_load, ";", 3);

	energy = stoi(ArchEvoGenUtil::split_string(regs, ",", 0));
	logo   = stoi(ArchEvoGenUtil::split_string(regs, ",", 1));
	guess  = stoi(ArchEvoGenUtil::split_string(regs, ",", 2));
	reg_a  = stoi(ArchEvoGenUtil::split_string(regs, ",", 3));
	reg_b  = stoi(ArchEvoGenUtil::split_string(regs, ",", 4));
	reg_c  = stoi(ArchEvoGenUtil::split_string(regs, ",", 5));
	reg_d  = stoi(ArchEvoGenUtil::split_string(regs, ",", 6));
	iploc  = stoi(ArchEvoGenUtil::split_string(regs, ",", 7));

	age            = stoi(ArchEvoGenUtil::split_string(data, ",", 0));
	lineage_length = stoi(ArchEvoGenUtil::split_string(data, ",", 1));
	species_id     = stoi(ArchEvoGenUtil::split_string(data, ",", 2));
	virility       = stoi(ArchEvoGenUtil::split_string(data, ",", 3));
	id             = stoi(ArchEvoGenUtil::split_string(data, ",", 4));

	ip = stoi(ip_str);

	for (int i = 0; i < NUMBER_OF_GENES; i++)
	{
		genes[i] = stoi(ArchEvoGenUtil::split_string(genome, ",", i));
	}
	
}

void CellState::make_child(CellState parent, float mutation_rate)
{
	//cout << "MAKING A BABY" << endl;
	int mutations = 0;
	for (int gene = 0; gene < NUMBER_OF_GENES; gene++)
	{
		//cout << "\tParent Gene " << bitset<11>(parent.genes[gene]) << endl;
		for (int bit = 11; bit >= 0; --bit)
		{
			genes[gene] = genes[gene] << 1;
			float random_num = ((double)rand() / (RAND_MAX));
			int parent_bit = (parent.genes[gene] >> (bit)) & 0b1;
			//cout << "\t\tBit #" << bit << " : " << parent_bit << "R: " << random_num <<  endl;
			
			if (random_num > mutation_rate)
			{
				genes[gene] += parent_bit;
			}
			else
			{
				//cout << "\t\t\tMutated!" << endl;
				mutations++;
				if (parent_bit == 1)
				{
					genes[gene] += 0;
				}
				else
				{
					genes[gene] += 1;
				}
			}
			
			//cout << "\t\tCurrent Gene " << bitset<11>(genes[gene]) << endl;
		}
	}
	//cout << "TOTAL MUTATIONS: " << mutations << endl;
	//cout << "MUTATION RATE: " << (float)mutations / (float)(NUMBER_OF_GENES * 11) << endl;
	//logo = parent.logo;
	infinite_energy = parent.infinite_energy;
	lineage_length = parent.lineage_length + 1;
	id = next_id++;
}