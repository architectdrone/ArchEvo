#pragma once
#include "CellState.h"
#include "libtcod.hpp"
#include "ISA.h"

#define DRAW_LINEAGE 0
#define DRAW_SPECIES 1

class Viewer
{
private:
	static int size;
	static void draw_cell(int x, int y, CellState* cell);
	static void draw_background(int x, int y, CellState*** world);
	static bool fast_forward;
public:
	static void init(int size); //Starts the window
	static void draw(CellState*** world); //Draws the world

	static int draw_mode;
};