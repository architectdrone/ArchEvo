#include "FastSpeciesContainer.h"

vector<vector<int>> FastSpeciesContainer::condense()
{
	vector<vector<int>> condensed_list;
	for (int i = 0; i < container.size(); i++)
	{
		if (container[i] != 0)
		{
			vector<int> new_entry;
			new_entry.resize(2);
			new_entry[0] = i+lowest;
			new_entry[1] = container[i];
			condensed_list.push_back(new_entry);
		}
	}
	return condensed_list;
}

void FastSpeciesContainer::uncondense(vector<vector<int>> condensed)
{
	for (int i = 0; i < condensed.size(); i++)
	{
		set(condensed[i][0], condensed[i][1]);
	}
}

int FastSpeciesContainer::get_lowest()
{
	return lowest;
}

int FastSpeciesContainer::get_set_size()
{
	return lowest + container.size();
}

float FastSpeciesContainer::average()
{
	int sum = 0;
	for (int i = 0; i < container.size(); i++)
	{
		sum += container[i];
	}
	return (float)sum / (float)get_set_size();
}

int FastSpeciesContainer::get(int i)
{
	int true_position = i - lowest;
	if (true_position < 0)
	{
		return 0;
	}
	else if (true_position >= container.size())
	{
		return 0;
	}
	else
	{
		return container[true_position];
	}
}

void FastSpeciesContainer::set(int num, int i)
{
	if (lowest == -1)
	{
		container.push_back(num);
		lowest = i;
	}
	else if (i >= lowest)
	{
		int true_position = i - lowest;
		if (true_position >= container.size())
		{
			container.resize(true_position + 1);
			container[true_position] = 0;
		}
		container[true_position] = num;
	}
	else
	{
		int lowest_difference = lowest - i;
		for (int i = 0; i < lowest_difference; i++)
		{
			container.insert(container.begin(), 0);
		}
		container[0] = num;
		lowest = i;
	}
}

void FastSpeciesContainer::clear()
{
	container.clear();
}
