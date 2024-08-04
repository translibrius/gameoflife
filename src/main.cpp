#include "raylib.h"
#include "common.h"

enum GameState {
    pickingCells,
    simulating,
};


const std::vector<Pattern> patterns = {
    {"Empty", 15, {}},
    {"Gliders", 50, { 53, 101, 103, 152, 153, 256, 304, 306, 355, 356, 1017, 1018, 1067, 1068 } },
    {"GliderGun", 50, { 829, 877, 879, 917, 918, 925, 926, 939, 940, 966, 970, 975, 976, 989, 990,
    1005, 1006, 1015, 1021, 1025, 1026, 1055, 1056, 1065, 1069, 1071, 1072, 1077, 1079, 1115, 1121,
    1129, 1166, 1170, 1217, 1218, }},
    {"BlinkerPuffer1", 71, { 2097, 2098, 2099, 2103, 2104, 2105, 2136, 2154, 2168, 2171, 2173, 2176, 2206, 2207, 2208, 2212, 2213, 2214, 2218, 2219, 2220, 2224, 2225, 2226, 2234, 2239, 2247, 2252, 2277, 2279, 2280, 2283, 2286, 2288, 2291, 2294, 2295, 2297, 2304, 2305, 2306, 2310, 2318, 2322, 2323, 2324, 2349, 2350, 2351, 2354, 2362, 2365, 2366, 2367, 2375, 2377, 2378, 2381, 2389, 2392, 2393, 2395, 2420, 2421, 2422, 2425, 2433, 2436, 2437, 2438, 2447, 2448, 2449, 2452, 2460, 2463, 2464, 2465, 2491, 2492, 2493, 2496, 2504, 2507, 2508, 2509, 2518, 2519, 2520, 2523, 2531, 2534, 2535, 2536, 2562, 2563, 2567, 2570, 2572, 2575, 2579, 2580, 2589, 2590, 2594, 2597, 2599, 2602, 2606, 2607, 2639, 2640, 2644, 2645, 2666, 2667, 2671, 2672, 2852, 2853, 2854, 2856, 2857, 2858, 2950, 2951, 2952, 2954, 2955, 2956, }},
};

// Returns alive cells
int makeMap(std::vector<Cell>& cellMap, Vector2 cellSize, int gridCount, std::vector<short> pattern) {
    // Create initial cell state map
    int cellIndex = 0;
    int aliveCells = 0;
    std::vector<Cell> newMap = {};
    for (short i = 0; i < gridCount; i++) {
        for (short j = 0; j < gridCount; j++) {
            float x = (float)j * cellSize.x;
            float y = HEADER_HEIGHT + (float)i * cellSize.y;

            Vector2 position = { x, y };

            bool enabled = false;
            if (std::find(pattern.begin(), pattern.end(), cellIndex) != pattern.end()) {
                enabled = true;
                aliveCells++;
            }

            newMap.push_back(Cell(cellIndex, enabled, position, cellSize));
            cellIndex++;
        }
    }
    cellMap = newMap;
    return aliveCells;
}

#ifdef _DEBUG
int main(int argc, char* argv[]) {
#else
int WinMain() {
#endif
    int currentPatternIndex = 1;
    Pattern currentPattern = patterns.at(currentPatternIndex);

    short gridCount = currentPattern.gridCount;
    float simsPerSec = 10.0f;

    Vector2 cellSize = { (float)WINDOW_WIDTH / gridCount, (float)(WINDOW_HEIGHT - HEADER_HEIGHT) / gridCount };

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Game of Life");
    InitAudioDevice();
    SetTargetFPS(180);

    // Initial states
    GameState gameState = pickingCells;
    
    std::vector<Cell> cellMap;
    int simulationCounter = 0;
    double timeSinceLastSim = 1;
    int lastToggledID = -1;
    int aliveCellCount = 0;

    Sound newGenSound = LoadSound("resources/gen.wav");
    SetSoundVolume(newGenSound, 0.05f);

    Music bgMusicStream = LoadMusicStream("resources/bg_music.ogg");
    bgMusicStream.looping = true;
    SetMusicVolume(bgMusicStream, 0.05f);
    PlayMusicStream(bgMusicStream);

    Rectangle btnStart;
    btnStart.x = WINDOW_WIDTH / 2 - 50;
    btnStart.y = 10.0f;
    btnStart.width = 100;
    btnStart.height = 25;
    Color btnStartColor = gameState == pickingCells ? GREEN : RED;

    Rectangle btnRefresh;
    btnRefresh.x = WINDOW_WIDTH / 2 + 60;
    btnRefresh.y = 10.0f;
    btnRefresh.width = 100;
    btnRefresh.height = 25;
    const Color btnRefreshColor = DARKGRAY;

    Rectangle btnLeft;
    btnLeft.x = WINDOW_WIDTH / 2 - 170;
    btnLeft.y = 10.0f;
    btnLeft.width = 50;
    btnLeft.height = 25;

    Rectangle btnRight;
    btnRight.x = WINDOW_WIDTH / 2 + 170;
    btnRight.y = 10.0f;
    btnRight.width = 50;
    btnRight.height = 25;

    aliveCellCount = makeMap(cellMap, cellSize, currentPattern.gridCount, currentPattern.aliveCells);

    while (!WindowShouldClose()) {
        if (!IsWindowReady()) continue;

        UpdateMusicStream(bgMusicStream);

        int fps = GetFPS();
        std::string fpsText = "FPS: " + std::to_string(fps);
        std::string simulationText = "Simulation: " + std::to_string(simulationCounter);
        std::string gridText = "Grid: " + std::to_string(gridCount) + "x" + std::to_string(gridCount);
        std::string speedText = "Speed: " + std::to_string(simsPerSec) + " sims/s";
        std::string startStopText = gameState == pickingCells ? "Start" : "Stop";
        std::string cellCounter = "Alive cells: " + std::to_string(aliveCellCount);
        std::string presetText = "Preset: " + currentPattern.name;
        btnStartColor = gameState == pickingCells ? GREEN : RED;

        // Input
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            int mouseX = GetMouseX();
            int mouseY = GetMouseY();

            if (mouseY > HEADER_HEIGHT) {
                Cell* foundCell = findCell(cellMap, mouseX, mouseY);
                if (foundCell != nullptr && lastToggledID != foundCell->id) {
                    std::cout << "Clicked cell -- " << foundCell->id << "\n";
                    foundCell->toggle();
                    if (foundCell->alive) aliveCellCount++;
                    else aliveCellCount--;
                    lastToggledID = foundCell->id;
                }

                // User wants to draw new cells, pause sim
                if (gameState != pickingCells) {
                    gameState = pickingCells;
                }
            }
        }

        // Button handling
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            lastToggledID = -1;
            int mouseX = GetMouseX();
            int mouseY = GetMouseY();

            if (mouseY <= HEADER_HEIGHT) {
                bool wasStartBtnPressed = CheckCollisionPointRec({ (float)mouseX, (float)mouseY }, btnStart);
                bool wasReloadPressed = CheckCollisionPointRec({ (float)mouseX, (float)mouseY }, btnRefresh);
                bool wasLeftPressed = CheckCollisionPointRec({ (float)mouseX, (float)mouseY }, btnLeft);
                bool wasRightPressed = CheckCollisionPointRec({ (float)mouseX, (float)mouseY }, btnRight);

                if (wasStartBtnPressed) {
                    if (gameState == pickingCells) gameState = simulating;
                    else if (gameState == simulating) gameState = pickingCells;
                }
                else if (wasReloadPressed) {
                    gameState = pickingCells;
                    gridCount = currentPattern.gridCount;
                    cellSize = { (float)WINDOW_WIDTH / gridCount, (float)(WINDOW_HEIGHT - HEADER_HEIGHT) / gridCount };
                    aliveCellCount = makeMap(cellMap, cellSize, gridCount, currentPattern.aliveCells);
                }
                else if (wasLeftPressed) {
                    currentPatternIndex--;
                    if (currentPatternIndex < 0) currentPatternIndex = patterns.size() - 1;
                    currentPattern = patterns[currentPatternIndex];
                    gridCount = currentPattern.gridCount;
                    cellSize = { (float)WINDOW_WIDTH / gridCount, (float)(WINDOW_HEIGHT - HEADER_HEIGHT) / gridCount };
                    aliveCellCount = makeMap(cellMap, cellSize, gridCount, currentPattern.aliveCells);
                }
                else if (wasRightPressed) {
                    currentPatternIndex++;
                    if (currentPatternIndex >= patterns.size()) currentPatternIndex = 0;
                    currentPattern = patterns[currentPatternIndex];
                    gridCount = currentPattern.gridCount;
                    cellSize = { (float)WINDOW_WIDTH / gridCount, (float)(WINDOW_HEIGHT - HEADER_HEIGHT) / gridCount };
                    aliveCellCount = makeMap(cellMap, cellSize, gridCount, currentPattern.aliveCells);
                }
            }
        }

        // Grid size adjust with mouse wheel
        if ((int)GetMouseWheelMove() >= 1) {
            gridCount--;
            if (gridCount < MIN_GRID) gridCount = MIN_GRID;
            cellSize = { (float)WINDOW_WIDTH / gridCount, (float)(WINDOW_HEIGHT - HEADER_HEIGHT) / gridCount };
            aliveCellCount = 0;
            aliveCellCount = makeMap(cellMap, cellSize, gridCount, patterns[0].aliveCells);
        }
        else if ((int)GetMouseWheelMove() <= -1) {
            gridCount++;
            if (gridCount > MAX_GRID) gridCount = MAX_GRID;
            cellSize = { (float)WINDOW_WIDTH / gridCount, (float)(WINDOW_HEIGHT - HEADER_HEIGHT) / gridCount };
            aliveCellCount = 0;
            aliveCellCount = makeMap(cellMap, cellSize, gridCount, patterns[0].aliveCells);
        }

        // Simulation speed control
        if (IsKeyDown(KEY_LEFT)) {
            simsPerSec -= 0.05f;
            if (simsPerSec < 0) simsPerSec = 0;
        }
        else if (IsKeyDown(KEY_RIGHT)) {
            simsPerSec += 0.05f;
        }
        else if (IsKeyPressed(KEY_SPACE)) {
            printAliveIDS(cellMap);
        }

        if (gameState == simulating) {
            if (timeSinceLastSim >= 1 / simsPerSec) {
                const short min = 0;
                const short max = cellMap.size();
                const short width = WINDOW_WIDTH / cellSize.x;  // Number of columns in the grid
                int nextAliveCounter = 0;

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

                    if (nextGen.at(i).alive) {
                        nextAliveCounter++;
                    }
                }
                aliveCellCount = nextAliveCounter;
                cellMap = nextGen;
                simulationCounter++;
                timeSinceLastSim = 0;
                if (simsPerSec < 20) PlaySound(newGenSound);
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

        // Top left header texts
        DrawText(fpsText.c_str(), 15, 10, 16, { 255, 255, 255, 255 });
        DrawText(simulationText.c_str(), 15, 25, 16, { 255, 255, 255, 255 });
        DrawText(gridText.c_str(), 15, 40, 16, { 255, 255, 255, 255 });
        DrawText(speedText.c_str(), 15, 55, 16, { 255, 255, 255, 255 });

        // Bottom center header text
        DrawText(cellCounter.c_str(), WINDOW_WIDTH / 2 - 80, HEADER_HEIGHT - 25, 20, { 255, 255, 255, 255 });
        DrawText(presetText.c_str(), WINDOW_WIDTH / 2 + 120, HEADER_HEIGHT - 25, 20, {255, 255, 255, 255});

        // Start / Stop button
        DrawRectangleRec(btnStart, btnStartColor);
        DrawText(startStopText.c_str(), WINDOW_WIDTH / 2 - 20, 15, 16, { 255, 255, 255, 255 });

        // Refresh button
        DrawRectangleRec(btnRefresh, btnRefreshColor);
        DrawText("Reset", WINDOW_WIDTH / 2 + 80, 15, 16, { 255, 255, 255, 255 });

        // Pattern navigation buttons
        DrawRectangleRec(btnLeft, btnRefreshColor);
        DrawText("<", WINDOW_WIDTH / 2 - 160, 15, 16, { 255, 255, 255, 255 });
        DrawRectangleRec(btnRight, btnRefreshColor);
        DrawText(">", WINDOW_WIDTH / 2 + 180, 15, 16, { 255, 255, 255, 255 });

        EndDrawing();
    }

    UnloadSound(newGenSound);
    UnloadMusicStream(bgMusicStream);

    CloseAudioDevice();
    CloseWindow();
}
