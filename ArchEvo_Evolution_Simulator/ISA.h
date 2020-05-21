#pragma once
#include "CellState.h"
#include <string>
using namespace std;
class ISA
{   
    private:
        static int iploc_x(int x, int y, int iploc);
        static int iploc_y(int x, int y, int iploc);
        static int get_bit(int byte, int bit_num);
        static void attack(int attacker_x, int attacker_y, int victim_x, int victim_y, CellState*** world_state);
        static void reproduce(int parent_x, int parent_y, int child_x, int child_y, CellState*** world_state);
        static int find(int x, int y, CellState*** world_state, int initial_ip);
        static void set_reg(int x, int y, int reg, int new_value, CellState*** world_state, int world_size);
        static int get_reg(int x, int y, int reg, CellState*** world_state, int world_size);
        static int get_int_from_bits(int byte, int first_bit, int last_bit);
        static int get_R1(int instruction);
        static int get_R2(int instruction);
        static int get_OP(int instruction);
        static bool is_COP(int instruction);
        static string get_register_name(int reg);
        static string get_R1_name(int instruction);
        static string get_R2_name(int instruction);
        static string get_cell_op_name(int cell_op);
        static string get_reg_op_name(int reg_op);
        
    public:
        static void execute(int x, int y, CellState*** world_state, int world_size);
        static string get_instruction_name(int instruction);
};

