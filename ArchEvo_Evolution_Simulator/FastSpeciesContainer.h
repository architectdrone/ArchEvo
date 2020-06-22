#pragma once
#include <vector>
using namespace std;
class FastSpeciesContainer
{
	private:
		vector<int> container;
		int lowest = -1;

	public:
		int get(int i);
		void set(int num, int i);

		void clear();

		vector<vector<int>> condense();
		void uncondense(vector<vector<int>> input);

		int get_lowest();
		int get_set_size();
		float average();
};

