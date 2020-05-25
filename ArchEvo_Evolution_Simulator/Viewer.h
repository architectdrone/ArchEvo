#pragma once
#include "CellState.h"
#include "WorldState.h"
#include "libtcod.hpp"
#include "ISA.h"

#define DRAW_LINEAGE 0
#define DRAW_SPECIES 1

#define MAIN_WINDOW_W 60
#define MAIN_WINDOW_H 40
#define WORLD_WINDOW_X 0
#define WORLD_WINDOW_Y 2
#define WORLD_WINDOW_W 25
#define WORLD_WINDOW_H 25
#define SPECIES_SCOREBOARD_X 0
#define SPECIES_SCOREBOARD_Y WORLD_WINDOW_Y+WORLD_WINDOW_H
#define SPECIES_SCOREBOARD_LINES 10
#define SPECIES_SCOREBOARD_LINE_WIDTH_LIVING 5
#define SPECIES_SCOREBOARD_LINE_WIDTH_EXTINCT 10
#define SPECIES_SCOREBOARD_H SPECIES_SCOREBOARD_LINES+1
#define SPECIES_SCOREBOARD_W SPECIES_SCOREBOARD_LINE_WIDTH_LIVING+SPECIES_SCOREBOARD_LINE_WIDTH_EXTINCT+3
#define CELL_DISPLAY_LINE_WIDTH 10
#define CELL_DISPLAY_X WORLD_WINDOW_X+WORLD_WINDOW_W
#define CELL_DISPLAY_Y WORLD_WINDOW_Y
#define CELL_DISPLAY_W 16
#define CELL_DISPLAY_H 4
#define REGISTER_DISPLAY_X CELL_DISPLAY_X
#define REGISTER_DISPLAY_Y CELL_DISPLAY_Y+CELL_DISPLAY_H
#define REGISTER_DISPLAY_REGISTER_BITS 8
#define REGISTER_DISPLAY_REP_WIDTH 3
#define REGISTER_DISPLAY_REGISTER_REP_DISTANCE 1 //Distance between register and representation
#define REGISTER_DISPLAY_X_DISTANCE 2 //Distance between physical and virtual registers
#define REGISTER_DISPLAY_NAME_WIDTH 7
#define REGISTER_DISPLAY_W REGISTER_DISPLAY_NAME_WIDTH+2*(REGISTER_DISPLAY_REGISTER_BITS+REGISTER_DISPLAY_REP_WIDTH+REGISTER_DISPLAY_REGISTER_REP_DISTANCE)+REGISTER_DISPLAY_X_DISTANCE+2
#define REGISTER_DISPLAY_H 13

#define SPEED_REAL_TIME 0 //Render the screen as fast as possible.
#define SPEED_SLOW 1 //Update the world once every few ticks
#define SPEED_PAUSED 2 //Pause updating
#define SPEED_FAST_FORWARD 3 //Skip rendering the screen

class Viewer
{
private:
	static void draw_cell(int x, int y, WorldState* world);
	static void draw_background(int x, int y, WorldState* world);
	static bool fast_forward;

	static void update_world(WorldState* world);
	static void update_status(WorldState* world);
	static void update_species_scoreboard(WorldState* world);
	static void update_cell_display(WorldState* world);
	static void update_register_display(WorldState* world);

	static TCODColor get_species_color(Species* the_species);
	static char get_species_char(Species* the_species);
	static void draw_species_icon(TCODConsole* console, int x, int y, Species* species);

	static TCODConsole* world_window;
	static TCODConsole* status_bar;
	static TCODConsole* species_scoreboard;
	static TCODConsole* cell_display;
	static TCODConsole* register_display;
	static int world_offset_x;
	static int world_offset_y;

	static int mouse_x;
	static int mouse_y;
	static bool click;

	static int cell_id;
	static int cell_x;
	static int cell_y;
	static CellState* the_cell;
public:
	static void init(); //Starts the window
	static void draw(WorldState* world); //Draws the world

	static int draw_mode;
	static bool highlights;
	static int speed;
};