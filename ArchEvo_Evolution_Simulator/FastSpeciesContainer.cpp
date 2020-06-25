#include "FastSpeciesContainer.h"

vector<vector<int>> FastSpeciesContainer::condense()
{
	vector<vector<int>> condensed_list;
	for (pair<int, int> element : container)
	{
		if (element.second != 0)
		{
			vector<int> new_entry;
			new_entry.resize(2);
			new_entry[0] = element.first;
			new_entry[1] = element.second;
			condensed_list.push_back(new_entry);
		}
	}
	return condensed_list;
}

vector<int> FastSpeciesContainer::simple_condense()
{
	vector<vector<int>> condensed_list = condense();
	vector<int> simple_condensed_list;

	for (int i = 0; i < condensed_list.size(); i++)
	{
		simple_condensed_list.push_back(condensed_list[i][0]);
	}

	return simple_condensed_list;
}

void FastSpeciesContainer::uncondense(vector<vector<int>> condensed)
{
	for (int i = 0; i < condensed.size(); i++)
	{
		set(condensed[i][0], condensed[i][1]);
	}
}

float FastSpeciesContainer::average()
{
	int sum = 0;
	int total = 0;
	for (pair<int, int> element : container)
	{
		total += element.first;
		sum   += element.first * element.second;
	}
	return (float)sum / (float)total;
}

int FastSpeciesContainer::get(int i)
{
	return container[i];
}

void FastSpeciesContainer::set(int num, int i)
{
	container[i] = num;
}

void FastSpeciesContainer::clear()
{
	container.clear();
}
