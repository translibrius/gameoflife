#pragma once
#include "raylib.h"
#include <string>
#include <vector>
#include <chrono>
#include <iostream>

#include "models.h"

static const Color COLOR_CELL_BORDER_DEAD = { 230, 41, 55, 90 };  // Red
static const Color COLOR_CELL_BORDER_ALIVE = { 230, 41, 55, 255 }; // Gold
static const Color COLOR_CELL_DEAD = { 20, 20, 0, 255 };			// Black
static const Color COLOR_CELL_ALIVE = { 100, 20, 0, 200 };			// Red-ish
static const Color COLOR_BACKGROUND = { 30, 30, 30, 255 };			// Black-ish

static const short CELL_BORDER_WIDTH = 3;
static const short CELL_PADDING = 1;

static const short WINDOW_WIDTH = 720;
static const short WINDOW_HEIGHT = 800;
static const short HEADER_HEIGHT = 80;

static const short MIN_GRID = 3;
static const short MAX_GRID = 120;
