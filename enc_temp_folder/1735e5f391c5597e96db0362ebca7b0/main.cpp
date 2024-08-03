#include "raylib.h"
#include <string>
#include <vector>
#include <iostream>

static const Color COLOR_CELL_DEAD = {20, 20, 0, 255};
static const Color COLOR_CELL_ALIVE = { 0, 200, 40, 255 };

static const Color COLOR_BACKGROUND = { 30, 30, 30, 255 };
const short CELL_SIZE = 20;

struct Cell {
	Cell(bool alive, Vector2 pos) {
		this->alive = alive;
		this->position = pos;
	};

	Cell(bool alive, Vector2 pos, short size) {
		this->alive = alive;
		this->position = pos;
		this->size = size;
	};

	bool alive;
	Vector2 position;
	short size = CELL_SIZE;

	void toggle() {
		this->alive = !alive;
	}

	void draw() {
		if (!alive) DrawRectangle(position.x, position.y, size, size, COLOR_CELL_DEAD);
		else		DrawRectangle(position.x, position.y, size, size, COLOR_CELL_ALIVE);
	}
};

Cell* findCell(std::vector<Cell> &cellList, int mouseX, int mouseY) {
	for (Cell& cell : cellList) {
		if (mouseX >= cell.position.x && mouseX <= cell.position.x + cell.size) {
			if (mouseY >= cell.position.y && mouseY <= cell.position.y + cell.size) {
				// Found cell
				std::cout << "Cell found at " << cell.position.x << ", " << cell.position.y << "\n";
				return &cell;
			}
		}
	}

	std::cout << "Could not find cell\n";
	return nullptr;
}

int main(int argc, char* argv[]) {
	const short WINDOW_WIDTH = 960;
	const short WINDOW_HEIGHT = 540;

	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Game of Life");
	SetTargetFPS(180);

	std::vector<Cell> cellMap;

	// Create initial cell state map
	for (short i = 0; i < WINDOW_HEIGHT / CELL_SIZE; i++) {
		for (int j = 0; j < WINDOW_WIDTH / CELL_SIZE; j++) {
			float x = (float) j * CELL_SIZE;
			float y = (float) i * CELL_SIZE;
			Vector2 position = { x, y };
			cellMap.push_back(Cell(false, position));
		}
	}

	while (!WindowShouldClose()) {
		if (!IsWindowReady()) continue;

		int fps = GetFPS();
		std::string fpsText = "FPS: " + std::to_string(fps);
		std::string startStopText = "Start";

		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
			int mouseX = GetMouseX();
			int mouseY = GetMouseY();
			std::cout << "Clicked X: " << mouseX << " Y: " << mouseY << "\n";
			Cell* foundCell = findCell(cellMap, mouseX, mouseY);
			if (foundCell != nullptr) foundCell->toggle();
		}

		// Draw
		BeginDrawing();
		ClearBackground(COLOR_BACKGROUND);
		

		for (int i = 0; i < cellMap.size(); i++) {
			Cell cell = cellMap.at(i);
			cell.draw();
		}

		// FPS indicator
		DrawText(fpsText.c_str(), WINDOW_WIDTH / 2, 20, 24, { 255, 0, 0, 255 });
		
		// Start / Stop button
		DrawRectangle(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT - 30, 100, 25, { 255, 0, 0, 255 });
		DrawText(startStopText.c_str(), WINDOW_WIDTH / 2 - 20, WINDOW_HEIGHT - 25, 16, {255, 255, 255, 255});

		EndDrawing();
	}

	CloseWindow();
}