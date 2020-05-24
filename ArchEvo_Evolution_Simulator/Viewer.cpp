#include "Viewer.h"
#include "Species.h"
#include <iostream>
int Viewer::draw_mode = DRAW_LINEAGE;
bool Viewer::fast_forward = false;
int Viewer::world_offset_x = 0;
int Viewer::world_offset_y = 0;
int Viewer::speed = SPEED_REAL_TIME;
bool Viewer::highlights = true;
TCODConsole* Viewer::world_window = new TCODConsole(WORLD_WINDOW_W, WORLD_WINDOW_H);
TCODConsole* Viewer::status_bar = new TCODConsole(MAIN_WINDOW_W, 2);
TCODConsole* Viewer::species_scoreboard = new TCODConsole(SPECIES_SCOREBOARD_W, SPECIES_SCOREBOARD_H);
bool Viewer::click = false;
int Viewer::mouse_x = 0;
int Viewer::mouse_y = 0;
int Viewer::cell_id = -1;

void Viewer::draw_cell(int x, int y, WorldState* world)
{
	CellState* cell = world->get_cell(x, y);
	char cell_char;
	TCODColor cell_color;
	int lineage = cell->lineage_length;
	int cell_s;
	Species* cell_species = nullptr;
	switch (draw_mode)
	{
		case DRAW_LINEAGE:
			if (cell->lineage_length == 0)
			{
				cell_char = '~';
			}
			else
			{
				cell_char = '0' + (lineage % 10);
			}
			
			cell_s = ((int)(lineage / 10)) * 10;
			if (cell_s > 100)
			{
				cell_s = 100;
			}

			cell_color = TCODColor::green;
			cell_color.setSaturation((float)cell_s/100.0f);
			cell_color.setValue(1.0f);
			break;
		case DRAW_SPECIES:
			
			cell_species = world->species_tracker.get_species(cell->species_id);
			cell_color = get_species_color(cell_species);
			cell_char = get_species_char(cell_species);
			break;
			
		default:
			cell_char = 'C';
			cell_color = TCODColor::red;
	}
	world_window->putChar(x, y, cell_char);
	world_window->setCharForeground(x, y, cell_color);
}

void Viewer::draw_background(int x, int y, WorldState* world)
{
	if (highlights)
	{
		vector<int> attacking = ISA::is_attacking(x, y, world);
		vector<int> reproducing = ISA::is_reproducing(x, y, world);
		if (attacking[0] != -1)
		{
			if (world->get_cell(attacking[0], attacking[1]) != nullptr)
			{
				world_window->setCharBackground(attacking[0], attacking[1], TCODColor::red);
				world_window->setCharBackground(x, y, TCODColor::lighterRed);
			}
		}
		else if (reproducing[0] != -1)
		{
			if (world->get_cell(reproducing[0], reproducing[1]) == nullptr && world->get_cell(x, y)->energy > INITIAL_ENERGY + 1)
			{
				world_window->setCharBackground(reproducing[0], reproducing[1], TCODColor::green);
				world_window->setCharBackground(x, y, TCODColor::lighterGreen);
			}
		}
	}

	if (cell_id == world->get_cell(x, y)->id)
	{
		world_window->setCharBackground(x, y, TCODColor::amber);
	}
}

void Viewer::update_world(WorldState* world)
{
	bool mouse_in_range = false;
	int world_mouse_x = mouse_x - WORLD_WINDOW_X;
	int world_mouse_y = mouse_y - WORLD_WINDOW_Y;
	if (mouse_x < WORLD_WINDOW_X + WORLD_WINDOW_W &&
		mouse_y < WORLD_WINDOW_Y + WORLD_WINDOW_H &&
		mouse_x > WORLD_WINDOW_X &&
		mouse_y > WORLD_WINDOW_Y)
	{
		mouse_in_range = true;
		world_window->setCharBackground(world_mouse_x, world_mouse_y, TCODColor::amber);
	}
	for (int x_i = 0; x_i < world_window->getWidth(); x_i++)
	{
		for (int y_i = 0; y_i < world_window->getHeight(); y_i++)
		{
			int x = x_i + world_offset_x;
			int y = y_i + world_offset_y;
			
			if (world->get_cell(x, y) != nullptr)
			{
				if (world_mouse_x == x_i && world_mouse_y == y_i && click)
				{
					cell_id = world->get_cell(x, y)->id;
				}
				draw_cell(x, y, world);
				draw_background(x, y, world);
			}
		}
	}
	
}

void Viewer::update_status(WorldState* world)
{
	//Status Bar
	status_bar->printf(0, 0, "ArchEvo");
	status_bar->printf(0, 1, "Iteration %d", world->get_iteration());
	string speed_string = "";
	switch (speed)
	{
	case SPEED_REAL_TIME:
		speed_string = "REAL";
		break;
	case SPEED_SLOW:
		speed_string = "SLOW";
		break;
	case SPEED_PAUSED:
		speed_string = "STOP";
		break;
	case SPEED_FAST_FORWARD:
		speed_string = "FAST";
		break;
	}
	string display_string = "";
	switch (draw_mode)
	{
	case DRAW_LINEAGE:
		display_string = "LINE";
		break;
	case DRAW_SPECIES:
		display_string = "SPEC";
		break;
	default:
		display_string = "BASC";
		break;
	}
	string highlight_string = "";
	if (highlights)
	{
		highlight_string = "HIGH";
	}
	else
	{
		highlight_string = "NONE";
	}
	status_bar->printf(status_bar->getWidth() - 1, 0, status_bar->getBackgroundFlag(), TCOD_RIGHT, "%s", speed_string.c_str());
	status_bar->printf(status_bar->getWidth() - 1, 1, status_bar->getBackgroundFlag(), TCOD_RIGHT, "%s|%s", highlight_string.c_str(), display_string.c_str());

	
}

void Viewer::update_species_scoreboard(WorldState* world)
{
	species_scoreboard->clear();
	species_scoreboard->printf(0, 0, "LIVING");
	species_scoreboard->printf(SPECIES_SCOREBOARD_LINE_WIDTH_LIVING + 2, 0, "DEAD");

	world->species_tracker.ensure_sorted();
	for (int i = 0; i < SPECIES_SCOREBOARD_LINES; i++)
	{
		Species* alive = world->species_tracker.get_species_rank(i);
		Species* extinct = world->species_tracker.get_species_rank_extinct(i);

		int living_species_id_x = 0;
		int living_species_id_y = 1 + i;
		int living_species_score_x = living_species_id_x + SPECIES_SCOREBOARD_LINE_WIDTH_LIVING;
		int living_species_score_y = living_species_id_y;
		int extinct_species_id_x = living_species_id_x + SPECIES_SCOREBOARD_LINE_WIDTH_LIVING+2;
		int extinct_species_id_y = living_species_id_y;
		int extinct_species_score_x = extinct_species_id_x + SPECIES_SCOREBOARD_LINE_WIDTH_EXTINCT;
		int extinct_species_score_y = extinct_species_id_y;

		if (alive != nullptr)
		{
			species_scoreboard->putChar(living_species_id_x, living_species_id_y, get_species_char(alive));
			species_scoreboard->setCharForeground(living_species_id_x, living_species_id_y, get_species_color(alive));
			species_scoreboard->printf(living_species_score_x, living_species_id_y, species_scoreboard->getBackgroundFlag(), TCOD_RIGHT, "%d", alive->get_alive());
		}
		else if (i == 0)
		{
			species_scoreboard->printf(living_species_id_x, living_species_id_y, "None");
		}

		if (extinct != nullptr)
		{
			species_scoreboard->putChar(extinct_species_id_x, extinct_species_id_y, get_species_char(extinct));
			species_scoreboard->setCharForeground(extinct_species_id_x, extinct_species_id_y, get_species_color(extinct));
			species_scoreboard->printf(extinct_species_score_x, extinct_species_id_y, species_scoreboard->getBackgroundFlag(), TCOD_RIGHT, "%d", extinct->get_total_alive());
		}
		else if (i == 0)
		{
			species_scoreboard->printf(extinct_species_id_x, extinct_species_id_y, "None");
		}
		
		//Divider
		species_scoreboard->putChar(extinct_species_id_x - 1, extinct_species_id_y, '|');
	}
}

TCODColor Viewer::get_species_color(Species* the_species)
{
	TCODColor to_return;
	if (the_species != nullptr)
	{
		to_return = TCODColor::green;
		to_return.setHue(((float)((char)(the_species->readable_id[0]) - 'a') / 26.0f) * 360);
	}
	else
	{
		to_return = TCODColor::white;
	}
	return to_return;
}

char Viewer::get_species_char(Species* the_species)
{
	char to_return;
	if (the_species != nullptr)
	{
		to_return = the_species->readable_id.back();
	}
	else
	{
		to_return = '~';
	}
	return to_return;
}

void Viewer::init()
{
	TCODConsole::initRoot(MAIN_WINDOW_W, MAIN_WINDOW_H, "ArchEvo Viewer", false);
	draw_mode = DRAW_LINEAGE;
}

void Viewer::draw(WorldState* world)
{
	TCODConsole::root->clear();
	TCOD_key_t key;
	TCOD_mouse_t mouse;
	TCODSystem::checkForEvent(TCOD_EVENT_ANY, &key, &mouse);

	switch (key.c)
	{
		case 'r':
			speed = SPEED_REAL_TIME;
			break;
		case 's':
			speed = SPEED_SLOW;
			break;
		case 'p':
			speed = SPEED_PAUSED;
			break;
		case 'f':
			speed = SPEED_FAST_FORWARD;
			break;
		case 'd':
			draw_mode = (draw_mode + 1) % 3;
			break;
		case 'h':
			highlights = !highlights;
			break;
	}

	
	world_window->clear();
	mouse_x = mouse.cx;
	mouse_y = mouse.cy;
	click = mouse.lbutton_pressed;
	if (speed != SPEED_FAST_FORWARD || world->get_iteration() % 1000 == 0)
	{
		//Update world display
		update_world(world);
		update_status(world);
		update_species_scoreboard(world);

		TCODConsole::blit(world_window, 0, 0, world_window->getWidth(), world_window->getHeight(), TCODConsole::root, WORLD_WINDOW_X, WORLD_WINDOW_Y);
		TCODConsole::blit(status_bar, 0, 0, status_bar->getWidth(), status_bar->getHeight(), TCODConsole::root, 0, 0);
		TCODConsole::blit(species_scoreboard, 0, 0, species_scoreboard->getWidth(), species_scoreboard->getHeight(), TCODConsole::root, SPECIES_SCOREBOARD_X, SPECIES_SCOREBOARD_Y);

		

		TCODConsole::flush();
	}
}
