#include "Viewer.h"
#include "Species.h"
int Viewer::draw_mode = DRAW_LINEAGE;
bool Viewer::fast_forward = false;
int Viewer::world_offset_x = 0;
int Viewer::world_offset_y = 0;
int Viewer::speed = SPEED_REAL_TIME;
bool Viewer::highlights = true;
TCODConsole* Viewer::world_window = new TCODConsole(WORLD_WINDOW_W, WORLD_WINDOW_H);
TCODConsole* Viewer::status_bar = new TCODConsole(MAIN_WINDOW_W, 2);

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
			if (cell_species != nullptr)
			{
				cell_color = TCODColor::green;
				cell_color.setHue(((float)((char)(cell_species->readable_id[0]) - 'a') / 26.0f) * 360);
				cell_char = cell_species->readable_id.back();
			}
			else
			{
				cell_color = TCODColor::white;
				cell_char = '~';
			}
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
}

void Viewer::update_world(WorldState* world)
{
	for (int x_i = 0; x_i < world_window->getWidth(); x_i++)
	{
		for (int y_i = 0; y_i < world_window->getHeight(); y_i++)
		{
			int x = x_i + world_offset_x;
			int y = y_i + world_offset_y;

			if (world->get_cell(x, y) != nullptr)
			{
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

void Viewer::init()
{
	TCODConsole::initRoot(MAIN_WINDOW_W, MAIN_WINDOW_H, "ArchEvo Viewer", false);
	draw_mode = DRAW_LINEAGE;
}

void Viewer::draw(WorldState* world)
{
	TCODConsole::root->clear();
	TCOD_key_t key;
	TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL);

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
	if (speed != SPEED_FAST_FORWARD || world->get_iteration() % 1000 == 0)
	{
		//Update world display
		update_world(world);
		update_status(world);
		
		TCODConsole::blit(world_window, 0, 0, world_window->getWidth(), world_window->getHeight(), TCODConsole::root, WORLD_WINDOW_X, WORLD_WINDOW_Y);
		TCODConsole::blit(status_bar, 0, 0, status_bar->getWidth(), status_bar->getHeight(), TCODConsole::root, 0, 0);
		
		TCODConsole::flush();
	}
}
