#pragma once
#include "CellState.h"
#include "Species.h"
#include "WorldState.h"
#include <string>
#include <vector>
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

class ISA
{   
    private:
        static int get_bit(int byte, int bit_num);
        static void attack(int attacker_x, int attacker_y, int victim_x, int victim_y, WorldState* world);
        static bool reproduce(int parent_x, int parent_y, int child_x, int child_y, WorldState* world);
        static int find(int x, int y, WorldState* world, int initial_ip);
        static void set_reg(int x, int y, int reg, int new_value, WorldState* world);
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
        //static void init();
        static void execute(int x, int y, WorldState* world);
        static void print_info(CellState* cell);
        static void print_genome(CellState* cell);
        static string get_instruction_name(int instruction);
        static int create_instruction(int OP);
        static int create_instruction(int OP, int R, bool cop = false);
        static int create_instruction(int OP, int R1, int R2);

        static vector<int> is_reproducing(int x, int y, WorldState* world);
        static vector<int> is_attacking(int x, int y, WorldState* world);

        static int get_reg(int x, int y, int reg, WorldState* world, bool blind = false, bool iploc = false);
        static int iploc_x(int x, int y, int iploc);
        static int iploc_y(int x, int y, int iploc);
};

