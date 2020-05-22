#include "Viewer.h"
#include "Species.h"
int Viewer::draw_mode = DRAW_LINEAGE;
int Viewer::size = 0;

void Viewer::draw_cell(int x, int y, CellState* cell)
{
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
			
			cell_species = ISA::get_species(cell->species_id);
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
	TCODConsole::root->putChar(x, y, cell_char);
	TCODConsole::root->setCharForeground(x, y, cell_color);
}

void Viewer::init(int _size)
{
	TCODConsole::initRoot(_size, _size, "ArchEvo Viewer", false);
	size = _size;
	draw_mode = DRAW_LINEAGE;
}

void Viewer::draw(CellState*** world)
{
	TCODConsole::root->clear();
	TCOD_key_t key;
	TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL);
	for (int x = 0; x < size; x++)
	{
		for (int y = 0; y < size; y++)
		{

			if (world[x][y] != nullptr)
			{
				draw_cell(x, y, world[x][y]);
			}
		}
	}
	TCODConsole::flush();
}