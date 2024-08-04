#pragma once
#include "common.h"

struct Cell {
    Cell(int id, bool alive, Vector2 pos, Vector2 size);

    int id;
    bool alive;
    Vector2 position;
    Vector2 size;

    void toggle();
    void draw() const;
};

struct Pattern {
    std::string name;
    int gridCount;
    std::vector<short> aliveCells;
};

Cell* findCell(std::vector<Cell>& cellList, int mouseX, int mouseY);
void printAliveIDS(std::vector<Cell> cellMap);