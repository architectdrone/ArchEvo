#include "GenebankGenerator.h"
#include "WorldState.h"
#include "SpeciesTracker.h"
#include <fstream>
#include <filesystem>
#include <bitset>
#include <algorithm>
#include "ISA.h"
#include <iostream>
SpeciesTracker GenebankGenerator::species_tracker;
using namespace std;
string abbreviate_string(string to_abbreviate)
{
	if (to_abbreviate.length() < 30)
	{
		return to_abbreviate;
	}
	else
	{
		string full_string = "";
		full_string += string(1, to_abbreviate[0]) + "...";
		for (int i = 26; i > 0; i--)
		{
			full_string += to_abbreviate[to_abbreviate.length() - i];
		}
		//cout << to_abbreviate << " -> " << full_string << endl;
		return full_string;
	}
}

struct display_line
{
	string name;
	int count;
	int id;
};

bool agg_comp(display_line a, display_line b)
{
	return a.count > b.count;
}

void GenebankGenerator::aggregate(ofstream& gene_file, vector<vector<int>> to_aggregate)
{
	int root_count = 0;
	vector<display_line> lines;

	display_line root;
	root.name = "ROOT";
	root.count = 0;
	root.id = 0;
	lines.push_back(root);
	
	for (int j = 0; j < to_aggregate.size(); j++)
	{
		Species* prey_species = species_tracker.get_species(to_aggregate[j][0], false);
		
		if (prey_species != nullptr)
		{
			string name = prey_species->readable_id;
			bool found = false;
			for (int k = 0; k < lines.size(); k++)
			{
				if (lines[k].name == prey_species->readable_id)
				{
					lines[k].count += to_aggregate[j][1];
					found = true;
				}
				
			}
			if (!found)
			{
				display_line new_line;
				new_line.name = prey_species->readable_id;
				new_line.count = to_aggregate[j][1];
				new_line.id = prey_species->id;
				lines.push_back(new_line);
			}
			//gene_file << "\t" << prey_species->readable_id << " : " << to_aggregate[j][1] << endl;
		}
		else
		{
			lines[0].count += to_aggregate[j][1];
		}
	}
	sort(lines.begin(), lines.end(), agg_comp);

	for (int i = 0; i < lines.size(); i++)
	{
		gene_file << "\t" << abbreviate_string(lines[i].name) << " (" << lines[i].id  << ") : " << lines[i].count << endl;
	}

}

void GenebankGenerator::generate_single_entry(Species* species, string path)
{
	ofstream gene_file;
	gene_file.open(path + "/" + to_string(species->id)+".txt");

	gene_file << "SPECIES #" << species->id << " '" << species->readable_id << "'" << endl;
	gene_file << "Lived from " << species->arrival_date << " - ";
	
	//Lifespan info
	if (species->get_extinction_date() != -1)
	{
		gene_file << species->get_extinction_date() << " (EXTINCT)" << endl;
	}
	else
	{
		gene_file << "PRESENT" << endl;
	}

	//Parent Info
	Species* parent_species = species_tracker.get_species(species->parent_id, false);
	if (parent_species != nullptr)
	{
		gene_file << "Parent: " << parent_species->readable_id << " (" << species->parent_id << ")" << endl;
	}
	else
	{
		gene_file << "Parent: ROOT" << endl;
	}

	//General Data
	gene_file << "Current: " << species->get_alive() << " Total: " << species->get_total_alive() << " Peak: " << species->get_peak_alive() << endl;
	gene_file << "Average Age: " << species->average_age() << " Average Verility: " << species->average_virility() << endl;
	gene_file << "Children: " << species->all_children().size() << endl;
	vector<vector<int>> prey = species->all_prey();
	gene_file << "Prey: " << endl;
	aggregate(gene_file, species->all_prey());
	gene_file << "Predators: " << endl;
	aggregate(gene_file, species->all_predators());

	gene_file << "=======================================================" << endl;
	for (int gene = 0; gene < NUMBER_OF_GENES; gene++)
	{
		gene_file << gene << " [" << bitset<11>(species->genome[gene]) << "] " << ISA::get_instruction_name(species->genome[gene]) << endl;
	}
}

void GenebankGenerator::generate_all(string filename)
{
	string extinct_species_path = WORLD_DIR + filename + "/species";
	cout << "Loading File..." << endl;
	species_tracker.load_state(extinct_species_path);
	vector<Species*> all_species = species_tracker.get_all_species();
	string gene_bank_path = WORLD_DIR + filename + "/genebank";
	if (!filesystem::exists(gene_bank_path))
	{
		//Create world directory
		filesystem::create_directory(gene_bank_path);
	}
	cout << "Creating Entries. Number of species: " << all_species.size() << endl;
	for (int i = 0; i < all_species.size(); i++)
	{
		cout << "\tCreating Entry " << i << ", " << all_species[i]->id << endl;
		generate_single_entry(all_species[i], gene_bank_path);
	}
}
