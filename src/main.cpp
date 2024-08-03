#include "raylib.h"

int main(int argc, char* argv[]) {
	InitWindow(500, 500, "Game of Life");

	while (!WindowShouldClose()) {
		if (!IsWindowReady()) continue;

		BeginDrawing();
		EndDrawing();
	}

	CloseWindow();
}