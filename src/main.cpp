#include "raylib.h"
#include <string>
#include <vector>
#include <iostream>
#include <chrono>

static const Color COLOR_CELL_DEAD = {20, 20, 0, 255};
static const Color COLOR_CELL_ALIVE = { 0, 200, 40, 255 };

static const Color COLOR_BACKGROUND = { 30, 30, 30, 255 };
const short CELL_SIZE = 20;

struct Cell {
	Cell(int id, bool alive, Vector2 pos) {
		this->alive = alive;
		this->position = pos;
		this->id = id;
	};

	Cell(bool alive, Vector2 pos, short size) {
		this->alive = alive;
		this->position = pos;
		this->size = size;
	};

	int id;
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

enum GameState {
	pickingCells,
	simulating,
};

Cell* findCell(std::vector<Cell> &cellList, int mouseX, int mouseY) {
	for (Cell& cell : cellList) {
		if (mouseX >= cell.position.x && mouseX <= cell.position.x + cell.size) {
			if (mouseY >= cell.position.y && mouseY <= cell.position.y + cell.size) {
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

int main(int argc, char* argv[]) {
	const short WINDOW_WIDTH = 960;
	const short WINDOW_HEIGHT = 540;
	const short HEADER_HEIGHT = 40;
	const float SIMS_PER_SECOND = 2.5f;

	const Vector2 btnStartPos { WINDOW_WIDTH / 2 - 50, 10.0f };
	const Vector2 btnStartSize{ 100, 25 };

	// 600 is center cell
	const std::vector<short> CELL_PATTERN_ROMBUS = { 840, 791, 793, 745, 743, 
													 694, 646, 598, 550, 502,
													 455, 407, 360, 409, 457,
												     506, 554, 602, 650, 698};

	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Game of Life");
	SetTargetFPS(180);

	GameState gameState = pickingCells;
	std::vector<Cell> cellMap;
	std::vector<short> defaultAlivePattern = CELL_PATTERN_ROMBUS;
	int simulationCounter = 0;
	double timeSinceLastSim = 1;

	// Create initial cell state map
	short cellIndex = 0;
	for (short i = HEADER_HEIGHT / CELL_SIZE; i < WINDOW_HEIGHT / CELL_SIZE; i++) {
		for (short j = 0; j < WINDOW_WIDTH / CELL_SIZE; j++) {
			float x = (float) j * CELL_SIZE;
			float y = (float) i * CELL_SIZE;
			Vector2 position = { x, y };

			bool enabled = std::find(defaultAlivePattern.begin(), defaultAlivePattern.end(), cellIndex) != defaultAlivePattern.end();
			cellMap.push_back(Cell(cellIndex, enabled, position));
			cellIndex++;
		}
	}

	while (!WindowShouldClose()) {
		if (!IsWindowReady()) continue;

		int fps = GetFPS();
		std::string fpsText = "FPS: " + std::to_string(fps);
		std::string startStopText = gameState == pickingCells ? "Start" : "Stop";

		// Input
		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
			int mouseX = GetMouseX();
			int mouseY = GetMouseY();
			std::cout << "Clicked X: " << mouseX << " Y: " << mouseY << "\n";

			if (mouseY > HEADER_HEIGHT) {
				if (gameState == pickingCells) {
					Cell* foundCell = findCell(cellMap, mouseX, mouseY);
					if (foundCell != nullptr) foundCell->toggle();
				}
			}
			else {
				bool wasStartBtnPressed = findStartBtn(btnStartPos, {(float) mouseX, (float) mouseY}, btnStartSize);
				if (wasStartBtnPressed) {
					if (gameState == pickingCells) gameState = simulating;
					else if (gameState == simulating) gameState = pickingCells;
				}
			}
		}
		
		if (gameState == simulating) {
			if (timeSinceLastSim >= 1 / SIMS_PER_SECOND) {
				const short min = 0;
				const short max = cellMap.size();
				const short width = WINDOW_WIDTH / CELL_SIZE;  // Number of columns in the grid

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
					short down = (row < (WINDOW_HEIGHT / CELL_SIZE) - 1) ? i + width : -1;

					short topLeft = (row > 0 && col > 0) ? i - width - 1 : -1;
					short topRight = (row > 0 && col < width - 1) ? i - width + 1 : -1;
					short bottomLeft = (row < (WINDOW_HEIGHT / CELL_SIZE) - 1 && col > 0) ? i + width - 1 : -1;
					short bottomRight = (row < (WINDOW_HEIGHT / CELL_SIZE) - 1 && col < width - 1) ? i + width + 1 : -1;

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

		// Draw
		BeginDrawing();
		ClearBackground(COLOR_BACKGROUND);

		for (int i = 0; i < cellMap.size(); i++) {
			Cell cell = cellMap.at(i);
			cell.draw();
		}

		// FPS indicator
		DrawText(fpsText.c_str(), 15, 10, 24, { 255, 0, 0, 255 });
		
		// Start / Stop button
		DrawRectangle(btnStartPos.x, btnStartPos.y, btnStartSize.x, btnStartSize.y, { 255, 0, 0, 255 });
		DrawText(startStopText.c_str(), WINDOW_WIDTH / 2 - 20, 15, 16, {255, 255, 255, 255});

		EndDrawing();
	}

	CloseWindow();
}