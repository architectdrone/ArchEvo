#include "ISA.h"
#include "CellState.h"
#include <cmath>
#include <string>
#include <iostream>
using namespace std;
#define ENERGY_REG  0b0000
#define LOGO_REG    0b0001
#define GUESS_REG   0b0010
#define A_REG       0b0011
#define B_REG       0b0100
#define C_REG	    0b0101
#define D_REG	    0b0110
#define IPLOC_REG   0b0111
#define ENERGY_IREG 0b1000
#define LOGO_IREG   0b1001
#define GUESS_IREG  0b1010
#define A_IREG      0b1011
#define B_IREG      0b1100
#define C_IREG	    0b1101
#define D_IREG	    0b1110
#define IPLOC_IREG  0b1111

#define INC_ROP 0b000 //Increase the value in R1 by 1
#define DEC_ROP 0b001 //Decrease the value in R1 by 1
#define SLL_ROP 0b010 //Shift R1 Left (Logical)
#define SRL_ROP 0b011 //Shift R1 Right (Logical)
#define MOV_ROP 0b100 //Move R2 to R1
#define SLT_ROP 0b101 //Set R1 to 0xFF if R1 < R2, else R1 = 0
#define SGT_ROP 0b110 //Set R1 to 0xFF if R1 > R2, else R1 = 0
#define SET_ROP 0b111 //Set R1 to 0xFF if R1 = R2, else R1 = 0

#define DIV_COP 0b000 //Reproduce at iploc
#define JMP_COP 0b001 //Jump to template
#define JPC_COP 0b010 //Jump to template if R2 = 0xFF
#define MOV_COP 0b011 //Move organism to iploc
#define IGN_COP 0b100 //(Literally does nothing)
#define NPA_COP 0b101 //NOP A, part of the template system.
#define NPB_COP 0b110 //NOP B, part of the template system.
#define ATK_COP 0b111 //Attack organism at iploc
using namespace std;
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
	else if (iploc >= 1)
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
	else if (iploc >= 1)
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

void ISA::reproduce(int parent_x, int parent_y, int child_x, int child_y, CellState*** world_state)
{
	world_state[child_x][child_y] = new CellState();
	world_state[child_x][child_y]->make_child(*world_state[parent_x][parent_y]);
}

int ISA::find(int x, int y, CellState*** world_state, int initial_ip)
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

	int R1_value = get_reg(x, y, R1, world_state, world_size);
	int R2_value = get_reg(x, y, R2, world_state, world_size);

	int target_x = iploc_x(x, y, current->iploc)%world_size;
	int target_y = iploc_y(x, y, current->iploc)%world_size;

	if (R1 == ENERGY_REG || (R1 > IPLOC_REG))
	{
		//Cell Operations
		if (op == DIV_COP)
		{
			reproduce(x, y, target_x, target_y, world_state);
		}
		else if (op == JMP_COP)
		{
			current->ip = find(x, y, world_state, current->ip);
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
	}
}

void ISA::set_reg(int x, int y, int reg, int new_value, CellState*** world_state, int world_size)
{
	CellState* current = world_state[x][y];
	if (reg == 0)
	{
		throw "Cannot Set Energy Directly!";
	}
	else if (reg = LOGO_REG)
	{
		current->logo = new_value;
	}
	else if (reg = GUESS_REG)
	{
		current->guess = new_value;
	}
	else if (reg == A_REG)
	{
		current->reg_a = new_value;
	}
	else if (reg == B_REG)
	{
		current->reg_b = new_value;
	}
	else if (reg == C_REG)
	{
		current->reg_c = new_value;
	}
	else if (reg == D_REG)
	{
		current->reg_d = new_value;
	}
	else if (reg == IPLOC_REG)
	{
		current->iploc = new_value;
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
	CellState* iploc_cell = world_state[iploc_x(x, y, current->iploc) % world_size][iploc_y(x, y, current->iploc) % world_size];
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
		int new_x = iploc_x(x, y, current->iploc) % world_size;
		int new_y = iploc_y(x, y, current->iploc) % world_size;
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
	for (int bit = 0; bit < 8; bit++)
	{
		if ((bit <= last_bit) && (bit >= first_bit))
		{
			mask += int(pow(2, mask));
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
	return get_int_from_bits(instruction, 6, 3);
}

int ISA::get_OP(int instruction)
{
	return get_int_from_bits(instruction, 2, 0);
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
	return get_register_name(get_R1(instruction));
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