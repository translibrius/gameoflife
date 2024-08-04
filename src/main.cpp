#include "raylib.h"
#include <string>
#include <vector>
#include <iostream>
#include <chrono>

static const Color COLOR_CELL_BORDER_DEAD = { 230, 41, 55, 90 };  // Red
static const Color COLOR_CELL_BORDER_ALIVE = { 230, 41, 55, 255 }; // Gold
static const short CELL_BORDER_WIDTH = 3;
static const short CELL_PADDING = 1;
static const Color COLOR_CELL_DEAD = {20, 20, 0, 255};
static const Color COLOR_CELL_ALIVE = { 100, 20, 0, 200 };
static const Color COLOR_BACKGROUND = { 30, 30, 30, 255 };

static const short WINDOW_WIDTH = 960;
static const short WINDOW_HEIGHT = 960;
static const short HEADER_HEIGHT = 80;

struct Cell {
	Cell(int id, bool alive, Vector2 pos, Vector2 size) {
		this->id = id,
		this->alive = alive;
		this->position = pos;
		this->size = size;
	};

	int id;
	bool alive;
	Vector2 position;
	Vector2 size;

	void toggle() {
		this->alive = !alive;
	}

	void draw() {
		// Border
		DrawRectangle(
			position.x + CELL_PADDING,
			position.y + CELL_PADDING,
			size.x - CELL_PADDING,
			size.y - CELL_PADDING,
			alive ? COLOR_CELL_BORDER_ALIVE : COLOR_CELL_BORDER_DEAD
		);
		// Cell
		DrawRectangle(
			position.x + CELL_BORDER_WIDTH + CELL_PADDING,
			position.y + CELL_BORDER_WIDTH + CELL_PADDING,
			size.x - CELL_BORDER_WIDTH - CELL_PADDING - 3,
			size.y - CELL_BORDER_WIDTH - CELL_PADDING - 3,
			alive ? COLOR_CELL_ALIVE : COLOR_CELL_DEAD
		);
	}
};

enum GameState {
	pickingCells,
	simulating,
};

Cell* findCell(std::vector<Cell> &cellList, int mouseX, int mouseY) {
	for (Cell& cell : cellList) {
		if (mouseX >= cell.position.x && mouseX <= cell.position.x + cell.size.x) {
			if (mouseY >= cell.position.y && mouseY <= cell.position.y + cell.size.y) {
				// Found cell
				std::cout << "Cell " << cell.id << " found at " << cell.position.x << ", " << cell.position.y << "\n";
				return &cell;
			}
		}
	}

	std::cout << "Could not find cell\n";
	return nullptr;
}

bool findStartBtn(Vector2 posBtn, Vector2 posClick, Vector2 btnSize) {
	if (posClick.x >= posBtn.x && posClick.x <= posBtn.x + btnSize.x) {
		if (posClick.y >= posBtn.y && posClick.y <= posBtn.y + btnSize.y) {
			return true;
		}
	}
	return false;
}

void makeMap(std::vector<Cell> &cellMap, Vector2 cellSize) {
	// Create initial cell state map
	short cellIndex = 0;
	std::vector<Cell> newMap = {};
	for (short i = HEADER_HEIGHT / cellSize.y; i < WINDOW_HEIGHT / cellSize.y; i++) {
		for (short j = 0; j < WINDOW_WIDTH / cellSize.x; j++) {
			float x = (float)j * cellSize.x;
			float y = (float)i * cellSize.y;
			Vector2 position = { x, y };

			bool enabled = false;
			newMap.push_back(Cell(cellIndex, enabled, position, cellSize));
			cellIndex++;
		}
	}
	cellMap = newMap;
}

int main(int argc, char* argv[]) {
	short gridCount = 30;
	float simsPerSec = 2.5f;

	Vector2 cellSize = { WINDOW_WIDTH / gridCount, WINDOW_HEIGHT / gridCount};
	const Vector2 btnStartPos { WINDOW_WIDTH / 2 - 50, 10.0f };
	const Vector2 btnStartSize{ 100, 25 };

	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Game of Life");
	SetTargetFPS(180);

	GameState gameState = pickingCells;
	std::vector<Cell> cellMap;
	std::vector<short> defaultAlivePattern = {};
	int simulationCounter = 0;
	double timeSinceLastSim = 1;
	int lastToggledID = -1;

	// Create initial cell state map
	short cellIndex = 0;
	makeMap(cellMap, cellSize);

	while (!WindowShouldClose()) {
		if (!IsWindowReady()) continue;

		int fps = GetFPS();
		std::string fpsText = "FPS: " + std::to_string(fps);
		std::string simulationText = "Simulation: " + std::to_string(simulationCounter);
		std::string gridText = "Grid: " + std::to_string(gridCount) + "x" + std::to_string(gridCount);
		std::string speedText = "Speed: " + std::to_string(simsPerSec) + " sims/s";
		std::string startStopText = gameState == pickingCells ? "Start" : "Stop";

		// Input
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			int mouseX = GetMouseX();
			int mouseY = GetMouseY();
			std::cout << "Clicked X: " << mouseX << " Y: " << mouseY << "\n";

			if (mouseY > HEADER_HEIGHT) {
				Cell* foundCell = findCell(cellMap, mouseX, mouseY);
				if (foundCell != nullptr && lastToggledID != foundCell->id) {
					foundCell->toggle();
					lastToggledID = foundCell->id;
				}

				// User wants to draw new cells, pause sim
				if (gameState != pickingCells) {
					gameState = pickingCells;
				}
			}
		}

		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
			int mouseX = GetMouseX();
			int mouseY = GetMouseY();

			if (mouseY <= HEADER_HEIGHT) {
				bool wasStartBtnPressed = findStartBtn(btnStartPos, { (float)mouseX, (float)mouseY }, btnStartSize);
				if (wasStartBtnPressed) {
					if (gameState == pickingCells) gameState = simulating;
					else if (gameState == simulating) gameState = pickingCells;
				}
			}
		}

		if ((int) GetMouseWheelMove() >= 1) {
			gridCount--;
			cellSize = { (float) WINDOW_WIDTH / gridCount, (float) WINDOW_HEIGHT / gridCount };
			makeMap(cellMap, cellSize);
		}
		else if ((int) GetMouseWheelMove() <= -1) {
			gridCount++;
			cellSize = { (float)WINDOW_WIDTH / gridCount, (float)WINDOW_HEIGHT / gridCount };
			makeMap(cellMap, cellSize);
		}

		if (IsKeyDown(KEY_LEFT)) {
			simsPerSec -= 0.05f;
			if (simsPerSec < 0) simsPerSec = 0;
		}
		else if (IsKeyDown(KEY_RIGHT)){
			simsPerSec += 0.05f;
		}
		
		if (gameState == simulating) {
			if (timeSinceLastSim >= 1 / simsPerSec) {
				const short min = 0;
				const short max = cellMap.size();
				const short width = WINDOW_WIDTH / cellSize.x;  // Number of columns in the grid

				std::vector<Cell> nextGen = cellMap;
				for (int i = min; i < max; i++) {
					Cell& cell = cellMap.at(i);
					short neighbors = 0;

					// Get row and column index of the current cell
					short row = i / width;
					short col = i % width;

					short left = (col > 0) ? i - 1 : -1;
					short right = (col < width - 1) ? i + 1 : -1;
					short up = (row > 0) ? i - width : -1;
					short down = (row < (WINDOW_HEIGHT / cellSize.y) - 1) ? i + width : -1;

					short topLeft = (row > 0 && col > 0) ? i - width - 1 : -1;
					short topRight = (row > 0 && col < width - 1) ? i - width + 1 : -1;
					short bottomLeft = (row < (WINDOW_HEIGHT / cellSize.y) - 1 && col > 0) ? i + width - 1 : -1;
					short bottomRight = (row < (WINDOW_HEIGHT / cellSize.y) - 1 && col < width - 1) ? i + width + 1 : -1;

					if (left > min && left < max && cellMap.at(left).alive) neighbors++;
					if (right > min && right < max && cellMap.at(right).alive) neighbors++;
					if (up > min && up < max && cellMap.at(up).alive) neighbors++;
					if (down > min && down < max && cellMap.at(down).alive) neighbors++;
					if (topLeft > min && topLeft < max && cellMap.at(topLeft).alive) neighbors++;
					if (topRight > min && topRight < max && cellMap.at(topRight).alive) neighbors++;
					if (bottomLeft > min && bottomLeft < max && cellMap.at(bottomLeft).alive) neighbors++;
					if (bottomRight > min && bottomRight < max && cellMap.at(bottomRight).alive) neighbors++;

					if (cell.alive) {
						if (neighbors < 2 || neighbors > 3) {
							nextGen.at(i).alive = false;
						}
					}
					else {
						if (neighbors == 3) {
							nextGen.at(i).alive = true;
						}
					}
				}
				cellMap = nextGen;
				simulationCounter++;
				timeSinceLastSim = 0;
			}
			else {
				timeSinceLastSim += GetFrameTime();
			}
		}
		else if (gameState == pickingCells) {
			simulationCounter = 0;
			timeSinceLastSim = 1;
		}

		// Draw -----------------------------------------------------------------------------------------
		BeginDrawing();
		ClearBackground(COLOR_BACKGROUND);

		for (int i = 0; i < cellMap.size(); i++) {
			Cell cell = cellMap.at(i);
			cell.draw();
		}

		// Top left texts
		DrawText(fpsText.c_str(), 15, 10, 16, { 255, 255, 255, 255 });
		DrawText(simulationText.c_str(), 15, 25, 16, { 255, 255, 255, 255 });
		DrawText(gridText.c_str(), 15, 40, 16, { 255, 255, 255, 255 });
		DrawText(speedText.c_str(), 15, 55, 16, { 255, 255, 255, 255 });
		
		// Start / Stop button
		DrawRectangle(btnStartPos.x, btnStartPos.y, btnStartSize.x, btnStartSize.y, { 255, 0, 0, 255 });
		DrawText(startStopText.c_str(), WINDOW_WIDTH / 2 - 20, 15, 16, {255, 255, 255, 255});

		EndDrawing();
	}

	CloseWindow();
}