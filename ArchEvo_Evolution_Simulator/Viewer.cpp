#include "Viewer.h"
#include "Species.h"
int Viewer::draw_mode = DRAW_LINEAGE;
bool Viewer::fast_forward = false;
int Viewer::world_offset_x = 0;
int Viewer::world_offset_y = 0;
TCODConsole* Viewer::world_window = new TCODConsole(WORLD_WINDOW_W, WORLD_WINDOW_H);

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
			
			cell_char = '0' + (lineage % 10);
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

	switch (key.vk)
	{
	case TCODK_SPACE:
		fast_forward = !fast_forward;
	}

	//Update world display
	world_window->clear();
	if (!fast_forward)
	{
		for (int x_i = 0; x_i < WORLD_WINDOW_W; x_i++)
		{
			for (int y_i = 0; y_i < WORLD_WINDOW_H; y_i++)
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
		TCODConsole::blit(world_window, 0, 0, WORLD_WINDOW_W, WORLD_WINDOW_H, TCODConsole::root, WORLD_WINDOW_X, WORLD_WINDOW_Y);
	}

	TCODConsole::root->printf(0, 0, "ArchEvo");
	TCODConsole::root->printf(0, 1, "Iteration %d", world->get_iteration());
	
	TCODConsole::flush();
}
