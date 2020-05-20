#pragma once
#include "CellState.h"
class ISA
{    
public:
    void execute(int x, int y, CellState*** world_state, int world_size);
};