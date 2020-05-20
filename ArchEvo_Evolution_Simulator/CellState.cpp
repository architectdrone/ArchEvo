#include "CellState.h"
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <cmath>
void CellState::make_random()
{
	for (int gene = 0; gene < NUMBER_OF_GENES; gene++)
	{
		for (int bit = 0; bit < 7; bit++)
		{
			if (rand() > 0.5)
			{
				genes[gene] += pow(2, bit);
			}
		}
	}
	logo = NVO_LOGO;
}

void CellState::make_child(CellState parent)
{
	int mutations = 0;
	for (int gene = 0; gene < NUMBER_OF_GENES; gene++)
	{
		for (int bit = 0; bit < 7; bit++)
		{
			if (rand() > MUTATION_RATE)
			{
				genes[gene] += pow(2, parent.genes[gene] >> bit);
			}
			else
			{
				mutations++;
				genes[gene] += pow(2, (parent.genes[gene] >> bit)+1);
			}
		}
	}
	logo = parent.logo;
}