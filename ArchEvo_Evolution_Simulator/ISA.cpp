#include "ISA.h"
#include <cmath>
using namespace std;
int iploc_x(int x, int y, int iploc)
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
	else if (iploc >= 1)
	{
		return x - 1;
	}
}

int iploc_y(int x, int y, int iploc)
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
	else if (iploc >= 1)
	{
		return y - 1;
	}
}

int get_bit(int byte, int bit_num)
{
	int exp = pow(2, bit_num);
	int masked = (byte & exp) >> bit_num;
}

void attack(int attacker_x, int attacker_y, int victim_x, int victim_y, CellState*** world_state)
{
	int guess = world_state[attacker_x][attacker_y]->guess;
	int damage = 0;
	if (world_state[victim_x][victim_y] != nullptr)
	{
		int logo = world_state[victim_x][victim_y]->logo;
		int correct_bits = -4;
		for (int i = 0; i < 8; i++)
		{
			if (get_bit(guess, i) == get_bit(logo, i))
			{
				correct_bits++;
			}
		}
		int damage = correct_bits - 4;
		//On perfect guess, do critical damage.
		if (correct_bits == 8)
		{
			damage *= 2;
		}
	}
	else
	{
		damage = -4;
		return;
	}
	

	
	

	world_state[attacker_x][attacker_y]->energy += damage;
	if (damage > 0)
	{
		world_state[victim_x][victim_y]->energy -= damage;
	}
}

void reproduce(int parent_x, int parent_y, int child_x, int child_y, CellState*** world_state)
{
	world_state[child_x][child_y] = new CellState();
	world_state[child_x][child_y]->make_child(*world_state[parent_x][parent_y]);
}

int find(int x, int y, CellState*** world_state, int initial_ip)
{
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
		if (current_op == 5 || current_op == 6)
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
				}

				current_template_length++;
				int initial_template_position = (initial_ip + current_template_length)%NUMBER_OF_GENES;
				if (current_template_length > initial_template_length)
				{
					testing_template = false;
					if (best_template_score < current_template_score)
					{
						best_template_score = current_template_score;
						best_template_start = current_template_start;
					}
				}
				if (current_cell->genes[initial_template_position] == current_cell->genes[current_ip])
				{
					current_template_score++;
				}
				
			}
		}
		else
		{
			if (in_initial_template)
			{
				in_initial_template = false;
			}
			else if (testing_template)
			{
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
		return initial_ip;
	}
	return best_template_start;
}

void ISA::execute(int x, int y, CellState*** world_state, int world_size)
{
	CellState* current = world_state[x][y];
	int instruction = current->genes[current->ip];

	int R1 = get_R1(instruction);
	int R2 = get_R2(instruction);
	int op = get_OP(instruction);

	int R1_value = get(x, y, R1, world_state, world_size);
	int R2_value = get(x, y, R2, world_state, world_size);

	int target_x = iploc_x(x, y, current->iploc)%world_size;
	int target_y = iploc_y(x, y, current->iploc)%world_size;

	if (R1 == 0 || (R1 >= 8 && R1 <= 15))
	{
		if (op == 0)
		{
			reproduce(x, y, target_x, target_y, world_state);
		}
		else if (op == 1)
		{
			current->ip = find(x, y, world_state, current->ip);
		}
		else if (op == 2)
		{
			if (R2_value == 0xFF)
			{
				current->ip = find(x, y, world_state, current->ip);
			}
		}
		else if (op == 3)
		{
			//Move
			CellState* temp = world_state[x][y];
			world_state[x][y] = world_state[target_x][target_y];
			world_state[target_x][target_y] = temp;
		}
		else if (op == 4)
		{
			//Literally do nothing
		}
		else if (op == 5)
		{
			//Nop A
		}
		else if (op == 6)
		{
			//Nop B
		}
		else if (op == 7)
		{
			attack(x, y, target_x, target_y, world_state);
		}
	}
	else
	{
		int new_R1 = 0;
		if (op == 0)
		{
			new_R1 = R1_value + 1;
		}
		else if (op == 1)
		{
			new_R1 = R1_value - 1;
		}
		else if (op == 2)
		{
			new_R1 = R1_value << 1;
		}
		else if (op == 3)
		{
			new_R1 = R1_value >> 1;
		}
		else if (op == 4)
		{
			new_R1 = R2_value;
		}
		else if (op == 5)
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
		else if (op == 6)
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
		else if (op == 7)
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
	}
}

void set(int x, int y, int reg, int new_value, CellState*** world_state, int world_size)
{
	CellState* current = world_state[x][y];
	if (reg == 0)
	{
		throw "Cannot Set Energy Directly!";
	}
	else if (reg == 1)
	{
		current->logo = new_value;
	}
	else if (reg == 2)
	{
		current->guess = new_value;
	}
	else if (reg == 3)
	{
		current->reg_a = new_value;
	}
	else if (reg == 4)
	{
		current->reg_b = new_value;
	}
	else if (reg == 5)
	{
		current->reg_c = new_value;
	}
	else if (reg == 6)
	{
		current->reg_d = new_value;
	}
	else if (reg == 7)
	{
		current->iploc = new_value;
	}
	else if (reg >= 8 && reg <= 15)
	{
		throw "Cannot set registers in another organism!";
	}
	else
	{
		throw "Incorrect Register!";
	}
}

int get(int x, int y, int reg, CellState*** world_state, int world_size)
{
	CellState* current = world_state[x][y];
	CellState* iploc_cell = world_state[iploc_x(x, y, current->iploc) % world_size][iploc_y(x, y, current->iploc) % world_size];
	if (reg == 0)
	{
		return current->energy;
	}
	else if (reg == 1)
	{
		return current->logo;
	}
	else if (reg == 2)
	{
		return current->guess;
	}
	else if (reg == 3)
	{
		return current->reg_a;
	}
	else if (reg == 4)
	{
		return current->reg_b;
	}
	else if (reg == 5)
	{
		return current->reg_c;
	}
	else if (reg == 6)
	{
		return current->reg_d;
	}
	else if (reg == 7)
	{
		return current->iploc;
	}
	else if (reg >= 8 && reg <= 15)
	{
		int new_x = iploc_x(x, y, current->iploc) % world_size;
		int new_y = iploc_y(x, y, current->iploc) % world_size;
		return get(new_x, new_y, reg - 8, world_state, world_size);
	}
	else
	{
		throw "Incorrect Register!";
	}
}

int get_int_from_bits(int byte, int first_bit, int last_bit)
{
	int mask = 0;
	for (int bit = 0; bit < 8; bit++)
	{
		if ((bit <= last_bit) && (bit >= first_bit))
		{
			mask += pow(2, mask);
		}
	}
	return (byte & mask) >> first_bit;
}

int get_R1(int instruction)
{
	return get_int_from_bits(instruction, 7, 10);
}

int get_R2(int instruction)
{
	return get_int_from_bits(instruction, 6, 3);
}

int get_OP(int instruction)
{
	return get_int_from_bits(instruction, 2, 0);
}