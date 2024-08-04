#include "models.h"
#include "common.h"

Cell::Cell(int id, bool alive, Vector2 pos, Vector2 size)
    : id(id), alive(alive), position(pos), size(size) {}

void Cell::toggle() {
    alive = !alive;
}

void Cell::draw() const {
    // Border
    DrawRectangle(
        position.x + CELL_PADDING,
        position.y + CELL_PADDING,
        size.x - 2 * CELL_PADDING,
        size.y - 2 * CELL_PADDING,
        alive ? COLOR_CELL_BORDER_ALIVE : COLOR_CELL_BORDER_DEAD
    );
    // Cell
    DrawRectangle(
        position.x + CELL_BORDER_WIDTH + CELL_PADDING,
        position.y + CELL_BORDER_WIDTH + CELL_PADDING,
        size.x - 2 * (CELL_BORDER_WIDTH + CELL_PADDING),
        size.y - 2 * (CELL_BORDER_WIDTH + CELL_PADDING),
        alive ? COLOR_CELL_ALIVE : COLOR_CELL_DEAD
    );
}
