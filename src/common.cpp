#include "common.h"

Cell* findCell(std::vector<Cell>& cellList, int mouseX, int mouseY) {
    for (Cell& cell : cellList) {
        if (mouseX >= cell.position.x && mouseX <= cell.position.x + cell.size.x) {
            if (mouseY >= cell.position.y && mouseY <= cell.position.y + cell.size.y) {
                // Found cell
                return &cell;
            }
        }
    }

    std::cout << "Could not find cell\n";
    return nullptr;
}

void printAliveIDS(std::vector<Cell> cellMap) {
    std::string output = "Alive: { ";
    for (Cell cell : cellMap) {
        if (cell.alive) output += std::to_string(cell.id) + ", ";
    }
    output += "}";
    std::cout << output << "\n";
}