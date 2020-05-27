#pragma once
#include <string>
#include "Species.h"
#include "SpeciesTracker.h"
using namespace std;
class GenebankGenerator
{
private:
	static void aggregate(ofstream& gene_file, vector<vector<int>> to_aggregate);
	static void generate_single_entry(Species* species, string filename);
	static SpeciesTracker species_tracker;
public:
	static void generate_all(string filename);
};