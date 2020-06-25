#pragma once
#include <vector>
#include <unordered_map>
#include <map>
using namespace std;
class FastSpeciesContainer
{
	private:
		unordered_map<int, int> container;
	public:
		int get(int i);
		void set(int num, int i);

		void clear();

		vector<vector<int>> condense();
		vector<int> simple_condense();
		void uncondense(vector<vector<int>> input);

		float average();
};

