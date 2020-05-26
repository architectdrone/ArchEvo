#include "Viewer.h"
#include "Species.h"
#include <iostream>
#include <bitset>
#include "ArchEvoGenUtil.h"
int Viewer::draw_mode = DRAW_LINEAGE;
bool Viewer::fast_forward = false;
int Viewer::world_offset_x = 0;
int Viewer::world_offset_y = 0;
int Viewer::speed = SPEED_REAL_TIME;
bool Viewer::highlights = true;
TCODConsole* Viewer::world_window = new TCODConsole(WORLD_WINDOW_W, WORLD_WINDOW_H);
TCODConsole* Viewer::status_bar = new TCODConsole(MAIN_WINDOW_W, 2);
TCODConsole* Viewer::species_scoreboard = new TCODConsole(SPECIES_SCOREBOARD_W, SPECIES_SCOREBOARD_H);
TCODConsole* Viewer::cell_display = new TCODConsole(CELL_DISPLAY_W, CELL_DISPLAY_H);
TCODConsole* Viewer::register_display = new TCODConsole(REGISTER_DISPLAY_W, REGISTER_DISPLAY_H);
TCODConsole* Viewer::genome_display = new TCODConsole(GENOME_DISPLAY_W, GENOME_DISPLAY_H);

bool Viewer::click = false;
int Viewer::mouse_x = 0;
int Viewer::mouse_y = 0;
int Viewer::cell_id = -1;
int Viewer::cell_x = 0;
int Viewer::cell_y = 0;
CellState* Viewer::the_cell = nullptr;

void Viewer::draw_cell(int x, int y, WorldState* world)
{
	int x_i = x - world_offset_x;
	int y_i = y - world_offset_y;
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
	world_window->putChar(x_i, y_i, cell_char);
	world_window->setCharForeground(x_i, y_i, cell_color);
}

void Viewer::draw_background(int x, int y, WorldState* world)
{
	int x_i = x - world_offset_x;
	int y_i = y - world_offset_y;
	if (highlights)
	{
		vector<int> attacking = ISA::is_attacking(x, y, world);
		vector<int> reproducing = ISA::is_reproducing(x, y, world);
		if (attacking[0] != -1)
		{
			if (world->get_cell(attacking[0], attacking[1]) != nullptr)
			{
				world_window->setCharBackground(attacking[0]-world_offset_x, attacking[1] - world_offset_y, TCODColor::red);
				world_window->setCharBackground(x_i, y_i, TCODColor::lighterRed);
			}
		}
		else if (reproducing[0] != -1)
		{
			if (world->get_cell(reproducing[0], reproducing[1]) == nullptr && world->get_cell(x, y)->energy > INITIAL_ENERGY + 1)
			{
				world_window->setCharBackground(reproducing[0] - world_offset_x, reproducing[1] - world_offset_y, TCODColor::green);
				world_window->setCharBackground(x_i, y_i, TCODColor::lighterGreen);
			}
		}
	}

	if (cell_id == world->get_cell(x, y)->id)
	{
		world_window->setCharBackground(x_i, y_i, TCODColor::amber);
	}
}

void Viewer::update_world(WorldState* world)
{
	//Update Mouse
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

	//Calculate World Offset
	world_offset_x = cell_x -(WORLD_WINDOW_W / 2);
	world_offset_y = cell_y -(WORLD_WINDOW_H / 2);
	the_cell = nullptr;
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
				if (cell_id == world->get_cell(x, y)->id)
				{
					cell_x = ArchEvoGenUtil::true_mod(x, world->get_size());
					cell_y = ArchEvoGenUtil::true_mod(y, world->get_size());
					the_cell = world->get_cell(x, y);
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
			//species_scoreboard->putChar(living_species_id_x, living_species_id_y, get_species_char(alive));
			//species_scoreboard->setCharForeground(living_species_id_x, living_species_id_y, get_species_color(alive));
			draw_species_icon(species_scoreboard, living_species_id_x, living_species_id_y, alive);
			species_scoreboard->printf(living_species_score_x, living_species_id_y, species_scoreboard->getBackgroundFlag(), TCOD_RIGHT, "%d", alive->get_alive());
		}
		else if (i == 0)
		{
			species_scoreboard->printf(living_species_id_x, living_species_id_y, "None");
		}

		if (extinct != nullptr)
		{
			draw_species_icon(species_scoreboard, extinct_species_id_x, extinct_species_id_y, extinct);
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

void Viewer::update_cell_display(WorldState* world)
{
	cell_display->clear();
	if (the_cell != nullptr)
	{
		cell_display->printf(0, 0, "SPECIES");
		Species* cell_species = world->species_tracker.get_species(the_cell->species_id);
		draw_species_icon(cell_display, CELL_DISPLAY_LINE_WIDTH, 0, cell_species);

		cell_display->printf(0, 1, "AGE");
		cell_display->printf(CELL_DISPLAY_LINE_WIDTH, 1, cell_display->getBackgroundFlag(), TCOD_RIGHT, "%d", the_cell->age);

		cell_display->printf(0, 2, "LINEAGE");
		cell_display->printf(CELL_DISPLAY_LINE_WIDTH, 2, cell_display->getBackgroundFlag(), TCOD_RIGHT, "%d", the_cell->lineage_length);
	
		cell_display->printf(0, 3, "VIRILITY");
		cell_display->printf(CELL_DISPLAY_LINE_WIDTH, 3, cell_display->getBackgroundFlag(), TCOD_RIGHT, "%d", the_cell->virility);
	}
	else
	{
		cell_display->printf(0, 0, "SELECT A CELL!");
	}
}

void print_bits_and_decimal_display(TCODConsole* console, int x, int y, int number)
{
	string bit_string = bitset<REGISTER_DISPLAY_REGISTER_BITS>(number).to_string();
	console->printf(x, y, "%s", bit_string.c_str());
	console->printf(x + REGISTER_DISPLAY_REGISTER_BITS+REGISTER_DISPLAY_REGISTER_REP_DISTANCE + REGISTER_DISPLAY_REP_WIDTH, y, console->getBackgroundFlag(), TCOD_RIGHT, "%d", number);
}

void Viewer::update_register_display(WorldState* world)
{
	register_display->clear();
	int name_column_start_x = 0;
	int binary_display_width = REGISTER_DISPLAY_REGISTER_BITS + REGISTER_DISPLAY_REP_WIDTH + REGISTER_DISPLAY_REGISTER_REP_DISTANCE + REGISTER_DISPLAY_X_DISTANCE;
	int cell_column_start_x = REGISTER_DISPLAY_NAME_WIDTH;
	int iploc_column_start_x = cell_column_start_x + binary_display_width;
	register_display->printf(cell_column_start_x , 0, "CELL");
	register_display->printf(iploc_column_start_x, 0, "IPLOC");

	//Make Name Column
	register_display->printf(name_column_start_x, 1, "ENERGY");
	register_display->printf(name_column_start_x, 2, "LOGO");
	register_display->printf(name_column_start_x, 3, "GUESS");
	register_display->printf(name_column_start_x, 4, "REG_A");
	register_display->printf(name_column_start_x, 5, "REG_B");
	register_display->printf(name_column_start_x, 6, "REG_C");
	register_display->printf(name_column_start_x, 7, "REG_D");
	register_display->printf(name_column_start_x, 8, "IPLOC");

	if (the_cell != nullptr)
	{
		print_bits_and_decimal_display(register_display, cell_column_start_x, 1, ISA::get_reg(cell_x, cell_y, ENERGY_REG, world));
		print_bits_and_decimal_display(register_display, cell_column_start_x, 2, ISA::get_reg(cell_x, cell_y, LOGO_REG, world));
		print_bits_and_decimal_display(register_display, cell_column_start_x, 3, ISA::get_reg(cell_x, cell_y, GUESS_REG, world));
		print_bits_and_decimal_display(register_display, cell_column_start_x, 4, ISA::get_reg(cell_x, cell_y, A_REG, world));
		print_bits_and_decimal_display(register_display, cell_column_start_x, 5, ISA::get_reg(cell_x, cell_y, B_REG, world));
		print_bits_and_decimal_display(register_display, cell_column_start_x, 6, ISA::get_reg(cell_x, cell_y, C_REG, world));
		print_bits_and_decimal_display(register_display, cell_column_start_x, 7, ISA::get_reg(cell_x, cell_y, D_REG, world));
		print_bits_and_decimal_display(register_display, cell_column_start_x, 8, ISA::get_reg(cell_x, cell_y, IPLOC_REG, world));

		print_bits_and_decimal_display(register_display, iploc_column_start_x, 1, ISA::get_reg(cell_x, cell_y, ENERGY_IREG, world));
		print_bits_and_decimal_display(register_display, iploc_column_start_x, 2, ISA::get_reg(cell_x, cell_y, LOGO_IREG, world));
		print_bits_and_decimal_display(register_display, iploc_column_start_x, 3, ISA::get_reg(cell_x, cell_y, GUESS_IREG, world));
		print_bits_and_decimal_display(register_display, iploc_column_start_x, 4, ISA::get_reg(cell_x, cell_y, A_IREG, world));
		print_bits_and_decimal_display(register_display, iploc_column_start_x, 5, ISA::get_reg(cell_x, cell_y, B_IREG, world));
		print_bits_and_decimal_display(register_display, iploc_column_start_x, 6, ISA::get_reg(cell_x, cell_y, C_IREG, world));
		print_bits_and_decimal_display(register_display, iploc_column_start_x, 7, ISA::get_reg(cell_x, cell_y, D_IREG, world));
		print_bits_and_decimal_display(register_display, iploc_column_start_x, 8, ISA::get_reg(cell_x, cell_y, IPLOC_IREG, world));
		
		//iploc display
		int physical_iploc_x = cell_column_start_x + binary_display_width - 3;
		int physical_iploc_y = 11;
		int iploc_iploc_x = iploc_column_start_x + binary_display_width - 3;
		int iploc_iploc_y = physical_iploc_y;

		int physcial_iploc_result_x = ISA::iploc_x(physical_iploc_x, physical_iploc_y, ISA::get_reg(cell_x, cell_y, IPLOC_REG, world));
		int physcial_iploc_result_y = ISA::iploc_y(physical_iploc_x, physical_iploc_y, ISA::get_reg(cell_x, cell_y, IPLOC_REG, world));
		int iploc_iploc_result_x = ISA::iploc_x(iploc_iploc_x, iploc_iploc_y, ISA::get_reg(cell_x, cell_y, IPLOC_IREG, world));
		int iploc_iploc_result_y = ISA::iploc_y(iploc_iploc_x, iploc_iploc_y, ISA::get_reg(cell_x, cell_y, IPLOC_IREG, world));

		register_display->putChar(physical_iploc_x, physical_iploc_y, 'C');
		register_display->putChar(iploc_iploc_x, iploc_iploc_y, 'C');

		register_display->putChar(physcial_iploc_result_x, physcial_iploc_result_y, 'I');
		register_display->putChar(iploc_iploc_result_x, iploc_iploc_result_y, 'I');
	}
}

void Viewer::update_genome_display(WorldState* world)
{
	genome_display->clear();
	if (the_cell == nullptr)
	{
		return;
	}
	int first_instruction = the_cell->ip-(GENOME_DISPLAY_LINES/2);

	int pointer_column_x = 0;
	int gene_column_x = pointer_column_x+2;
	int name_column_x = gene_column_x + GENOME_DISPLAY_INSTRUCTION_BITS + 1;
	for (int i = 0; i < GENOME_DISPLAY_LINES; i++)
	{
		int y = i;
		int ip = ArchEvoGenUtil::true_mod(i + first_instruction, NUMBER_OF_GENES);
		int instruction = the_cell->genes[ip];
		if (ip == the_cell->ip)
		{
			genome_display->printf(pointer_column_x, y, ">");
		}
		genome_display->printf(gene_column_x, y, "%s", bitset<GENOME_DISPLAY_INSTRUCTION_BITS>(instruction).to_string().c_str());
		genome_display->printf(name_column_x, y, "%s", ISA::get_instruction_name(instruction).c_str());
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

void Viewer::draw_species_icon(TCODConsole* console, int x, int y, Species* species)
{
	console->putChar(x, y, get_species_char(species));
	console->setCharForeground(x, y, get_species_color(species));
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
	TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS || TCOD_EVENT_MOUSE_MOVE || TCOD_EVENT_MOUSE_PRESS, &key, &mouse);

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
		update_cell_display(world);
		update_register_display(world);
		update_genome_display(world);

		TCODConsole::blit(world_window, 0, 0, world_window->getWidth(), world_window->getHeight(), TCODConsole::root, WORLD_WINDOW_X, WORLD_WINDOW_Y);
		TCODConsole::blit(status_bar, 0, 0, status_bar->getWidth(), status_bar->getHeight(), TCODConsole::root, 0, 0);
		TCODConsole::blit(species_scoreboard, 0, 0, species_scoreboard->getWidth(), species_scoreboard->getHeight(), TCODConsole::root, SPECIES_SCOREBOARD_X, SPECIES_SCOREBOARD_Y);
		TCODConsole::blit(cell_display, 0, 0, cell_display->getWidth(), cell_display->getHeight(), TCODConsole::root, CELL_DISPLAY_X, CELL_DISPLAY_Y);
		TCODConsole::blit(register_display, 0, 0, register_display->getWidth(), register_display->getHeight(), TCODConsole::root, REGISTER_DISPLAY_X, REGISTER_DISPLAY_Y);
		TCODConsole::blit(genome_display, 0, 0, genome_display->getWidth(), genome_display->getHeight(), TCODConsole::root, GENOME_DISPLAY_X, GENOME_DISPLAY_Y);

		TCODConsole::flush();
	}
}
