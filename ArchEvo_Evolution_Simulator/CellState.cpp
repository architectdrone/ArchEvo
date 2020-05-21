#include "CellState.h"
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <cmath>
#include <iostream>
#include <bitset>
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
	logo = NVO_LOGO;
}

void CellState::make_child(CellState parent)
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
			
			if (random_num > MUTATION_RATE)
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
	logo = parent.logo;
}