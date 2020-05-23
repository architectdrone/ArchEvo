#include "ISA.h"
#include "CellState.h"
#include <cmath>
#include <string>
#include <iostream>
#include <stdio.h>
#include <bitset>
using namespace std;
const char letters[26] = { 'a', 'b', 'c', 'd','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z' };
vector<Species*> ISA::species_list = vector<Species*>();
vector<Species*> ISA::extinct_species_list = vector<Species*>();
int ISA::next_species_id = 1;

Species* ISA::get_species(int species_id)
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
		if (species_list[i]->id == species_id)
		{
			return species_list[i];
		}
	}

	return nullptr;
}

vector<Species*> ISA::get_all_species()
{
	vector<Species*> to_return = species_list;
	to_return.insert(to_return.end(), extinct_species_list.begin(), extinct_species_list.end());

	return to_return;
}

int ISA::number_of_living_species()
{
	return species_list.size();
}

int ISA::number_of_extinct_species()
{
	return extinct_species_list.size();
}


void ISA::delete_species(int species_id)
{
	for (int i = 0; i < species_list.size(); i++)
	{
		if (species_list[i]->id == species_id)
		{
			delete species_list[i];
			species_list.erase(species_list.begin()+i);
			return;
		}
	}
}

/*
void ISA::init()
{
	Species first;
	first.id = 0;
	first.readable_id = "";
	species_list.push_back(first);
}
*/
int true_mod(int n, int m)
{
	return (m + (n % m)) % m;
}

int ISA::iploc_x(int x, int y, int iploc)
{
	if (iploc >= 128)
	{
		return x + 1;
	}
	else if(iploc >= 64)
	{
		return x + 0;
	}
	else if (iploc >= 32)
	{
		return x - 1;
	}
	else if (iploc >= 16)
	{
		return x + 1;
	}
	else if (iploc >= 8)
	{
		return x - 1;
	}
	if (iploc >= 4)
	{
		return x + 1;
	}
	else if (iploc >= 2)
	{
		return x + 0;
	}
	else
	{
		return x - 1;
	}
}

int ISA::iploc_y(int x, int y, int iploc)
{
	if (iploc >= 128)
	{
		return y + 1;
	}
	else if (iploc >= 64)
	{
		return y + 1;
	}
	else if (iploc >= 32)
	{
		return y + 1;
	}
	else if (iploc >= 16)
	{
		return y;
	}
	else if (iploc >= 8)
	{
		return y;
	}
	if (iploc >= 4)
	{
		return y - 1;
	}
	else if (iploc >= 2)
	{
		return y - 1;
	}
	else
	{
		return y - 1;
	}
}

int ISA::get_bit(int byte, int bit_num)
{
	int exp = int(pow(2, bit_num));
	return (byte & exp) >> bit_num;
}

void ISA::attack(int attacker_x, int attacker_y, int victim_x, int victim_y, CellState*** world_state)
{
	if (world_state[attacker_x][attacker_y] == nullptr)
	{
		throw "Empty cell attempts to attack?";
	}
	int guess = world_state[attacker_x][attacker_y]->guess;
	int damage = 0;
	if (world_state[victim_x][victim_y] != nullptr)
	{
		
		int logo = world_state[victim_x][victim_y]->logo;
		//cout << "Cell at " << attacker_x << ", " << attacker_y << " strikes! LOGO: " << bitset<8>(logo) << " GUESS: " << bitset<8>(guess) << endl;
		int correct_bits = 0;
		for (int i = 0; i < 8; i++)
		{
			int guess_bit = (guess >> i) & 0b1;
			int logo_bit = (logo >> i) & 0b1;
			if (guess_bit == logo_bit)
			{
				correct_bits++;
			}
		}
		damage = (world_state[victim_x][victim_y]->energy)/(9-correct_bits);
		//On perfect guess, do critical damage.
		if (correct_bits == 0)
		{
			damage = -5;
		}
	}
	else
	{
		damage = -4;
	}
	
	world_state[attacker_x][attacker_y]->energy += damage;
	if (damage > 0)
	{
		Species* attacker_species = get_species(world_state[attacker_x][attacker_y]->species_id);
		Species* victim_species = get_species(world_state[victim_x][victim_y]->species_id);
		if (attacker_species != nullptr)
		{
			attacker_species->register_eating(world_state[victim_x][victim_y]->species_id);
		}
		if (victim_species != nullptr)
		{
			victim_species->register_being_eaten(world_state[attacker_x][attacker_y]->species_id);
		}
		world_state[victim_x][victim_y]->energy -= damage;
	}
}

bool ISA::reproduce(int parent_x, int parent_y, int child_x, int child_y, CellState*** world_state, int date)
{
	if ((world_state[parent_x][parent_y]->energy > INITIAL_ENERGY+1) && (world_state[child_x][child_y] == nullptr))
	{
		//cout << "Cell has successfully reproduced. ";
		if (world_state[parent_x][parent_y]->lineage_length == 0)
		{
			//cout << "Ex Nihilo" << endl;
		}
		else
		{
			//cout << "CHILD OF CHILD!" << endl;
		}
		world_state[parent_x][parent_y]->energy -= INITIAL_ENERGY; //Deduct required energy
		world_state[parent_x][parent_y]->virility += 1;
		world_state[child_x][child_y] = new CellState();
		world_state[child_x][child_y]->make_child(*world_state[parent_x][parent_y]);
		world_state[child_x][child_y]->energy = INITIAL_ENERGY;

		Species* parent_species = get_species(world_state[parent_x][parent_y]->species_id);
		CellState* child = world_state[child_x][child_y];
		if (child->lineage_length > 2)
		{
			if (parent_species != 0 && parent_species->tolerable_difference(child))
			{
				//Member of species_list.
				parent_species->register_birth();
				child->species_id = parent_species->id;
			}
			else
			{
				//New species_list
				Species* new_species = new Species();
				new_species->arrival_date = date;
				new_species->id = next_species_id++;
				
				for (int gene = 0; gene < NUMBER_OF_GENES; gene++)
				{
					new_species->genome[gene] = child->genes[gene];
				}

				string original_name = "";
				if (parent_species != nullptr)
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
				
				new_species->readable_id = original_name + letters[rand() % 26];
				new_species->register_birth();
				species_list.push_back(new_species);
				child->species_id = new_species->id;
			}
		}
		
		return true;
		
	}
	else
	{
		return false;
	}
}

int ISA::find(int x, int y, CellState*** world_state, int initial_ip)
{
	/*
	Returns the location of the next closest template match from initial_ip. 
	*/
	int current_ip = initial_ip+1;
	bool in_initial_template = true;
	bool testing_template = false;
	int best_template_start = -1;
	int best_template_score = 0;
	int current_template_start = 0;
	int current_template_length = 0;
	int current_template_score = 0;
	int initial_template_length = 0;
	CellState* current_cell = world_state[x][y];
	while (current_ip != initial_ip)
	{
		int current_op = get_OP(current_cell->genes[current_ip]);
		if (current_op == NPA_COP || current_op == NPB_COP)
		{
			//NOP
			if (in_initial_template)
			{
				initial_template_length += 1;
			}
			else
			{
				//Testing another template
				if (!testing_template)
				{
					current_template_start = current_ip;
					current_template_length = 0;
					testing_template = true;
					//cout << "Testing template at " << current_template_start << endl;
				}

				current_template_length++;
				int initial_template_position = (initial_ip + current_template_length)%NUMBER_OF_GENES;
				if (current_template_length > initial_template_length)
				{
					//Current Template exceeds the length of original template.
					//cout << "Done with current template @ " << current_ip << ". Score = " << current_template_score << endl;
					testing_template = false;
					if (best_template_score < current_template_score)
					{
						best_template_score = current_template_score;
						best_template_start = current_template_start;
					}
					if (best_template_score == initial_template_length)
					{
						return best_template_start;
					}
				}
				//cout << "Current Template Length " << current_template_length << endl;
				//cout << get_instruction_name(current_cell->genes[initial_template_position-1]) << "?=" << get_instruction_name(current_cell->genes[current_ip]) << endl;
				if (current_cell->genes[initial_template_position-1] == current_cell->genes[current_ip])
				{
					current_template_score++;
				}
				
			}
		}
		else
		{
			if (in_initial_template)
			{
				//cout << "Length of original template " << initial_template_length << endl;
				in_initial_template = false;
			}
			else if (testing_template)
			{
				//Reached end of template
				//cout << "Done with current template @ " << current_ip << ". Score = " << current_template_score << endl;
				testing_template = false;
				if (best_template_score < current_template_score)
				{
					best_template_score = current_template_score;
					best_template_start = current_template_start;
				}
			}
		}
		current_ip++;
		current_ip = current_ip % NUMBER_OF_GENES;
	}
	if (best_template_start == -1)
	{
		//cout << "No other templates found" << endl;
		return initial_ip;
	}
	return best_template_start;
}

void ISA::execute(int x, int y, CellState*** world_state, int world_size, int date)
{
	CellState* current = world_state[x][y];
	if (current == nullptr)
	{
		return;
	}
	int instruction = current->genes[current->ip];

	//Kill cell if energy is gone.
	if (current->energy <= 0)
	{
		Species* the_species = nullptr;
		int species_pos = -1;
		for (int i = 0; i < species_list.size(); i++)
		{
			if (species_list[i]->id == current->species_id)
			{
				the_species = species_list[i];
				species_pos = i;
			}
		}
		
		if (the_species != nullptr)
		{
			the_species->register_dying(current, date);
			if (the_species->get_alive() == 0)
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
		

		delete world_state[x][y];
		world_state[x][y] = nullptr;
		return;
	}
	
	
	current->ip = ((current->ip) + 1) % NUMBER_OF_GENES; //Increment Instruction Pointer
	current->age += 1; //Increment Age

	//Names of the registers and operations
	int R1 = get_R1(instruction);
	int R2 = get_R2(instruction);
	int op = get_OP(instruction);

	//Values of the registers
	int R1_value = get_reg(x, y, R1, world_state, world_size);
	int R2_value = get_reg(x, y, R2, world_state, world_size);

	//IPLOC values
	int target_x = true_mod(iploc_x(x, y, current->iploc),world_size);
	int target_y = true_mod(iploc_y(x, y, current->iploc), world_size);

	if (R1 == ENERGY_REG || (R1 > IPLOC_REG))
	{
		//Cell Operations
		if (op == DIV_COP)
		{
			reproduce(x, y, target_x, target_y, world_state, date);
			
		}
		else if (op == JMP_COP)
		{
			current->ip = find(x, y, world_state, current->ip);
			//cout << "Jumping forward to " << current->ip << endl;
		}
		else if (op == JPC_COP)
		{
			if (R2_value == 0xFF)
			{
				current->ip = find(x, y, world_state, current->ip);
			}
		}
		else if (op == MOV_COP)
		{
			//Move
			CellState* temp = world_state[x][y];
			world_state[x][y] = world_state[target_x][target_y];
			world_state[target_x][target_y] = temp;
		}
		else if (op == IGN_COP)
		{
			//Literally do nothing
		}
		else if (op == NPA_COP)
		{
			//Nop A
		}
		else if (op == NPB_COP)
		{
			//Nop B
		}
		else if (op == ATK_COP)
		{
			attack(x, y, target_x, target_y, world_state);
		}
	}
	else
	{
		int new_R1 = 0;
		if (op == INC_ROP)
		{
			new_R1 = R1_value + 1;
		}
		else if (op == DEC_ROP)
		{
			new_R1 = R1_value - 1;
		}
		else if (op == SLL_ROP)
		{
			new_R1 = R1_value << 1;
		}
		else if (op == SRL_ROP)
		{
			new_R1 = R1_value >> 1;
		}
		else if (op == MOV_ROP)
		{
			new_R1 = R2_value;
		}
		else if (op == SLT_ROP)
		{
			if (R1_value < R2_value)
			{
				new_R1 = 0xFF;
			}
			else
			{
				new_R1 = 0x00;
			}
		}
		else if (op == SGT_ROP)
		{
			if (R1_value > R2_value)
			{
				new_R1 = 0xFF;
			}
			else
			{
				new_R1 = 0x00;
			}
		}
		else if (op == SET_ROP)
		{
			if (R1_value == R2_value)
			{
				new_R1 = 0xFF;
			}
			else
			{
				new_R1 = 0x00;
			}
		}
		set_reg(x, y, R1, new_R1, world_state, world_size);
	}
}

void ISA::set_reg(int x, int y, int reg, int new_value, CellState*** world_state, int world_size)
{
	int true_value = true_mod(new_value, 0b11111111);
	CellState* current = world_state[x][y];
	if (reg == 0)
	{
		throw "Cannot Set Energy Directly!";
	}
	else if (reg == LOGO_REG)
	{
		current->logo = true_value;
	}
	else if (reg == GUESS_REG)
	{
		current->guess = true_value;
	}
	else if (reg == A_REG)
	{
		current->reg_a = true_value;
	}
	else if (reg == B_REG)
	{
		current->reg_b = true_value;
	}
	else if (reg == C_REG)
	{
		current->reg_c = true_value;
	}
	else if (reg == D_REG)
	{
		current->reg_d = true_value;
	}
	else if (reg == IPLOC_REG)
	{
		current->iploc = true_value;
	}
	else if (reg == ENERGY_IREG || reg == LOGO_IREG || reg == GUESS_IREG || reg == A_IREG || reg == B_IREG || reg == C_IREG || reg == D_IREG || reg == IPLOC_IREG)
	{
		throw "Cannot set_reg registers in another organism!";
	}
	else
	{
		throw "Incorrect Register!";
	}
}

int ISA::get_reg(int x, int y, int reg, CellState*** world_state, int world_size)
{
	CellState* current = world_state[x][y];
	CellState* iploc_cell = world_state[true_mod(iploc_x(x, y, current->iploc), world_size)][true_mod(iploc_y(x, y, current->iploc), world_size)];
	if (reg == ENERGY_REG)
	{
		return current->energy;
	}
	else if (reg == LOGO_REG)
	{
		return current->logo;
	}
	else if (reg == GUESS_REG)
	{
		return current->guess;
	}
	else if (reg == A_REG)
	{
		return current->reg_a;
	}
	else if (reg == B_REG)
	{
		return current->reg_b;
	}
	else if (reg == C_REG)
	{
		return current->reg_c;
	}
	else if (reg == D_REG)
	{
		return current->reg_d;
	}
	else if (reg == IPLOC_REG)
	{
		return current->iploc;
	}
	else if (reg == ENERGY_IREG ||reg == LOGO_IREG ||reg == GUESS_IREG ||reg == A_IREG ||reg == B_IREG ||reg == C_IREG ||reg == D_IREG  ||reg == IPLOC_IREG)
	{
		int new_x = true_mod(iploc_x(x, y, current->iploc),  world_size);
		int new_y = true_mod(iploc_y(x, y, current->iploc),  world_size);
		if ((world_state[new_x][new_y]) == nullptr)
		{
			return 0;
		}
		return get_reg(new_x, new_y, reg - 8, world_state, world_size);
	}
	else
	{
		throw "Incorrect Register!";
	}
}

int ISA::get_int_from_bits(int byte, int first_bit, int last_bit)
{
	int mask = 0;
	for (int bit = 0; bit < 11; bit++)
	{	
		int bit_value = (byte >> bit) & 1;		
		if ((bit <= last_bit) && (bit >= first_bit))
		{
			mask += bit_value*int(pow(2, bit));
		}
	}
	return (byte & mask) >> first_bit;
}

int ISA::get_R1(int instruction)
{
	return get_int_from_bits(instruction, 7, 10);
}

int ISA::get_R2(int instruction)
{
	return get_int_from_bits(instruction, 3, 6);
}

int ISA::get_OP(int instruction)
{
	return get_int_from_bits(instruction, 0, 2);
}

bool ISA::is_COP(int instruction)
{
	int R1 = get_R1(instruction);
	return R1 == ENERGY_REG || R1 == ENERGY_IREG || R1 == LOGO_IREG || R1 == GUESS_IREG || R1 == A_IREG || R1 == B_IREG || R1 == C_IREG || R1 == D_IREG || R1 == IPLOC_IREG;
}

string ISA::get_register_name(int reg)
{
	switch (reg)
	{
	case ENERGY_REG:
		return "ENERGY";
	case GUESS_REG:
		return "GUESS";
	case IPLOC_REG:
		return "IPLOC";
	case LOGO_REG:
		return "LOGO";
	case A_REG:
		return "A";
	case B_REG:
		return "B";
	case C_REG:
		return "C";
	case D_REG:
		return "D";
	case ENERGY_IREG:
		return "I_ENERGY";
	case GUESS_IREG:
		return "I_GUESS";
	case IPLOC_IREG:
		return "I_IPLOC";
	case LOGO_IREG:
		return "I_LOGO";
	case A_IREG:
		return "I_A";
	case B_IREG:
		return "I_B";
	case C_IREG:
		return "I_C";
	case D_IREG:
		return "I_D";
	default:
		return "???";
	}
}

string ISA::get_R1_name(int instruction)
{
	return get_register_name(get_R1(instruction));
}

string ISA::get_R2_name(int instruction)
{
	return get_register_name(get_R2(instruction));
}

string ISA::get_cell_op_name(int cell_op)
{
	switch (cell_op)
	{
		case ATK_COP:
			return "ATTACK";
		case DIV_COP:
			return "REPRODUCE";
		case IGN_COP:
			return "NONE";
		case JMP_COP:
			return "JUMP";
		case JPC_COP:
			return "JUMP_COND";
		case NPA_COP:
			return "NOP_A";
		case NPB_COP:
			return "NOP_B";
		case MOV_COP:
			return "MOVE";
		default:
			return  "???";
	}
}

string ISA::get_reg_op_name(int reg_op)
{
	switch (reg_op)
	{
		case INC_ROP:
			return "INC";
		case DEC_ROP:
			return "DEC";
		case SLL_ROP:
			return "SLL";
		case SRL_ROP:
			return "SRL";
		case MOV_ROP:
			return "MOVE";
		case SGT_ROP:
			return "SET_IF_GREATER";
		case SLT_ROP:
			return "SET_IF_LESS";
		case SET_ROP:
			return "SET_IF_EQUAL";
		default:
			return  "???";
	}
}

string ISA::get_instruction_name(int instruction)
{
	int operation = get_OP(instruction);
	int R1 = get_R1(instruction);
	int R2 = get_R2(instruction);
	int num_parameters = 0;
	string op_name = "";
	bool parameter_R1 = false;
	bool parameter_R2 = false;
	if (is_COP(instruction))
	{
		op_name = get_cell_op_name(operation);
		if (operation == JPC_COP)
		{
			parameter_R2 = true;
		}
	}
	else
	{
		op_name = get_reg_op_name(operation);
		parameter_R1 = true;
		if (!(operation == INC_ROP || operation == DEC_ROP || operation == SLL_ROP || operation == SRL_ROP))
		{
			parameter_R2 = true;
		}
	}

	string text_instruction = op_name;
	if (parameter_R1)
	{
		text_instruction += " " + get_R1_name(instruction);
	}
	if (parameter_R2)
	{
		text_instruction += " " + get_R2_name(instruction);
	}

	return text_instruction;
}

int ISA::create_instruction(int OP)
{
	return create_instruction(OP, 0b0000, true);
}

int ISA::create_instruction(int OP, int R, bool cop)
{
	if (cop)
	{
		return create_instruction(OP, 0b0000, R);
	}
	else
	{
		return create_instruction(OP, R, 0b0000);
	}
}
int ISA::create_instruction(int OP, int R1, int R2)
{
	int instruction = 0;
	instruction += R1;
	instruction = instruction << 4;
	instruction += R2;
	instruction = instruction << 3;
	instruction += OP;
	return instruction;
}

void ISA::print_info(CellState* cell)
{
	if (cell == nullptr)
	{
		cout << "Empty..." << endl;
		return;
	}
	cout << "ENERGY: " << cell->energy << endl;
	cout << "LOGO: " << cell->logo << " GUESS: " << cell->guess << endl;
	cout << "IPLOC: " << cell->iploc << " (" << bitset<8>(cell->iploc) << " -> (" << iploc_x(0, 0, cell->iploc) << ", " << iploc_y(0, 0, cell->iploc) << "))" << endl;
	cout << "A: " << cell->reg_a << " B: " << cell->reg_b << " C: " << cell->reg_c << " D: " << cell->reg_d << endl;
	cout << "Age: " << cell->age << " Lineage: " << cell->lineage_length << " Species ID: " << cell->species_id << " Virility: " << cell->virility << endl;
	cout << "Next Instruction (" << cell->ip << "): [" << bitset<11>(cell->genes[cell->ip]) << "] " << get_instruction_name(cell->genes[cell->ip]) << endl;
}
void ISA::print_genome(CellState* cell)
{
	if (cell == nullptr)
	{
		cout << "Empty..." << endl;
		return;
	}
	for (int gene = 0; gene < NUMBER_OF_GENES; gene++)
	{
		cout << gene << " [" << bitset<11>(cell->genes[gene]) << "] " << get_instruction_name(cell->genes[gene]) << endl;
	}
}

void ISA::print_all_species()
{
	vector<Species*> all_species = get_all_species();
	for (int i = 0; i < all_species.size(); i++)
	{
		Species* species = all_species[i];
		if (species->get_extinction_date() == -1 || (species->all_children().size() != 0))
		{
			cout << "----------------------------" << endl;

			cout << "Species: " << species->readable_id << " (" << species->id << ")" << endl;
			cout << "Lived from " << species->arrival_date << " - ";
			if (species->get_extinction_date() != -1)
			{
				cout << species->get_extinction_date() << " (EXTINCT)" << endl;
			}
			else
			{
				cout << "PRESENT" << endl;
			}
			Species* parent_species = get_species(species->parent_id);
			if (parent_species != nullptr)
			{
				cout << "Parent: " << parent_species->readable_id << " (" << species->parent_id << ")" << endl;
			}
			else
			{
				cout << "Parent: ROOT" << endl;
			}
			
			cout << "Current: " << species->get_alive() << " Total: " << species->get_total_alive() << " Peak: " << species->get_peak_alive() << endl;
			cout << "Average Age: " << species->average_age() << " Average Verility: " << species->average_virility() << endl;
			cout << "Children: " << species->all_children().size() << endl;
			vector<vector<int>> prey = species->all_prey();
			cout << "Prey: " << endl;
			int root_count = 0;
			for (int j = 0; j < prey.size(); j++)
			{
				Species* prey_species = get_species(prey[j][0]);
				if (prey_species != nullptr)
				{
					cout << "\t" << prey_species->readable_id << " : " << prey[j][1] << endl;
				}
				else
				{
					root_count += prey[j][1];
				}
			}
			if (root_count != 0)
			{
				cout << "\tROOT: " << root_count << endl;
			}

			/*
			
			vector<vector<int>> predators = species->all_predators();
			cout << "Predators: " << endl;
			root_count = 0;
			for (int j = 0; j < predators.size(); j++)
			{
				Species* predator_species = get_species(predators[j][0]);
				if (predator_species != nullptr)
				{
					cout << "\t" << predator_species->readable_id << " : " << predators[j][1] << endl;
				}
				else
				{
					root_count += predators[j][1];
				}
			}
			if (root_count != 0)
			{
				cout << "\tROOT: " << root_count << endl;
			}
			cout << "-------------------------" << endl;
			*/
		}

	}
}

vector<int> ISA::is_reproducing(int x, int y, CellState*** world, int size)
{
	vector<int> to_return(2, -1);
	if (world[x][y] == nullptr)
	{
		return to_return;
	}
	CellState* cell = world[x][y];
	int instruction = cell->genes[cell->ip];
	if (is_COP(instruction) && get_OP(instruction) == DIV_COP)
	{
		to_return[0] = true_mod(iploc_x(x, y, cell->iploc), size);
		to_return[1] = true_mod(iploc_y(x, y, cell->iploc), size);
		return to_return;
	}
	else
	{
		return to_return;
	}
}

vector<int> ISA::is_attacking(int x, int y, CellState*** world, int size)
{
	vector<int> to_return(2, -1);
	if (world[x][y] == nullptr)
	{
		return to_return;
	}
	CellState* cell = world[x][y];
	int instruction = cell->genes[cell->ip];
	if (is_COP(instruction) && get_OP(instruction) == ATK_COP)
	{
		to_return[0] = true_mod(iploc_x(x, y, cell->iploc), size);
		to_return[1] = true_mod(iploc_y(x, y, cell->iploc), size);
		return to_return;
	}
	else
	{
		return to_return;
	}
}
