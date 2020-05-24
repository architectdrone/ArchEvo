#pragma once
#include "CellState.h"
#include "WorldState.h"
#include "libtcod.hpp"
#include "ISA.h"

#define DRAW_LINEAGE 0
#define DRAW_SPECIES 1

#define WORLD_WINDOW_W 25
#define WORLD_WINDOW_H 25
#define MAIN_WINDOW_W 40
#define MAIN_WINDOW_H 40
#define WORLD_WINDOW_X 2
#define WORLD_WINDOW_Y 2
#define SPECIES_SCOREBOARD_X 0
#define SPECIES_SCOREBOARD_Y WORLD_WINDOW_Y+WORLD_WINDOW_H
#define SPECIES_SCOREBOARD_LINES 10
#define SPECIES_SCOREBOARD_LINE_WIDTH_LIVING 5
#define SPECIES_SCOREBOARD_LINE_WIDTH_EXTINCT 10
#define SPECIES_SCOREBOARD_H SPECIES_SCOREBOARD_LINES+1
#define SPECIES_SCOREBOARD_W SPECIES_SCOREBOARD_LINE_WIDTH_LIVING+SPECIES_SCOREBOARD_LINE_WIDTH_EXTINCT+3


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

	static TCODColor get_species_color(Species* the_species);
	static char get_species_char(Species* the_species);

	static TCODConsole* world_window;
	static TCODConsole* status_bar;
	static TCODConsole* species_scoreboard;
	static int world_offset_x;
	static int world_offset_y;
public:
	static void init(); //Starts the window
	static void draw(WorldState* world); //Draws the world

	static int draw_mode;
	static bool highlights;
	static int speed;
};