#include "CellState.h"
#include "ISA.h"
#include <iostream>
using namespace std;
int main()
{
	int test_instruction = 0b00010011100; //MOVE LOGO RA
	cout << ISA::get_instruction_name(test_instruction) << endl;
	test_instruction = ISA::create_instruction(MOV_ROP, LOGO_REG, A_REG);
	cout << ISA::get_instruction_name(test_instruction) << endl;
	
}