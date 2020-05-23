#include "SpeciesTracker.h"
const char letters[26] = { 'a', 'b', 'c', 'd','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z' };

Species* SpeciesTracker::get_species(int species_id)
{
	if (species_id == 0)
	{
		return nullptr;
	}
	for (int i = 0; i < species_list.size(); i++)
	{
		if (species_list[i]->id == species_id)
		{
			return species_list[i];
		}
	}
	//Also search extinct species.
	for (int i = 0; i < extinct_species_list.size(); i++)
	{
		if (extinct_species_list[i]->id == species_id)
		{
			return extinct_species_list[i];
		}
	}
	//Finally, search for pruned extinct species. Return the parent of the pruned species.
	for (int i = 0; i < extinct_species_list.size(); i++)
	{
		Species* the_species = extinct_species_list[i];
		vector<int> all_subspecies = the_species->all_children();
		for (int j = 0; j < all_subspecies.size(); j++)
		{
			if (all_subspecies[j] == species_id)
			{
				return the_species;
			}
		}
	}

	//Who knows? Probably a flash in the pan species.
	return nullptr;
}

vector<Species*> SpeciesTracker::get_all_species()
{
	vector<Species*> to_return = species_list;
	to_return.insert(to_return.end(), extinct_species_list.begin(), extinct_species_list.end());

	return to_return;
}

int SpeciesTracker::number_of_living_species()
{
	return species_list.size();
}

int SpeciesTracker::number_of_extinct_species()
{
	return extinct_species_list.size();
}

void SpeciesTracker::prune_extinct_species()
{
	//Deletes all evolutionary dead end species, "collapsing" them into the parent species.
	for (int i = 0; i < extinct_species_list.size(); i++)
	{
		Species* the_species = extinct_species_list[i];
		if ((the_species->all_children()).size() == 0)
		{
			Species* parent_species = get_species(the_species->parent_id);
			parent_species->register_child_species(the_species->id);
			extinct_species_list.erase(extinct_species_list.begin() + i);
			delete the_species;
			i--;
		}
	}
}

void SpeciesTracker::create_new_species_if_needed(CellState* parent_cell, CellState* child_cell, int date)
{
	Species* parent_species = get_species(parent_cell->species_id);
	if (child_cell->lineage_length > 2) //Only consider cells that have reproduced twice.
	{
		bool new_root_species = parent_species == nullptr;
		bool mutated_species = parent_species != nullptr && !parent_species->tolerable_difference(child_cell); //The cell is a mutated version of another species.
		
		if (new_root_species || mutated_species)
		{
			//New species!
			Species* new_species = new Species();
			new_species->arrival_date = date;
			new_species->id = next_species_id++;

			//Populate species genome
			for (int gene = 0; gene < NUMBER_OF_GENES; gene++)
			{
				new_species->genome[gene] = child_cell->genes[gene];
			}

			string original_name = "";
			if (mutated_species)
			{
				original_name = parent_species->readable_id;
				new_species->parent_id = parent_species->id;
				parent_species->register_child_species(new_species->id);
			}
			else
			{
				original_name = "";
				new_species->parent_id = 0;
			}

			//Create the name
			new_species->readable_id = original_name + letters[rand() % 26];
			new_species->register_birth();
			species_list.push_back(new_species);
			child_cell->species_id = new_species->id;
		}
	}
}

void SpeciesTracker::extinction_if_needed(CellState* dead_cell, int date)
{
	Species* the_species = nullptr;
	int species_pos = -1;
	for (int i = 0; i < species_list.size(); i++)
	{
		if (species_list[i]->id == dead_cell->species_id)
		{
			the_species = species_list[i];
			species_pos = i;
		}
	}

	if (the_species != nullptr && the_species->get_alive() == 0)
	{
		//Extinction
		species_list.erase(species_list.begin() + species_pos); //Remove it from the species list.
		if ((the_species->get_total_alive() == 1) || (the_species->parent_id == 0 && the_species->all_children().size() == 0))
		{
			//We consider irrelevant all "flash in the pan" species. These species
			//1. Only ever had one member
			//2. Were a child of root and had no sub species.
			delete the_species;
		}
		else
		{
			//Otherwise, we move the species to an extinct list, so the program doesn't need to scan through them anymore.
			extinct_species_list.push_back(the_species);
		}
	}
}