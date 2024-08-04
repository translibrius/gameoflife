# Conway's Game of Life

This project is an implementation of Conway's Game of Life using the Raylib library. Conway's Game of Life is a cellular automaton devised by the British mathematician John Horton Conway in 1970. It consists of a grid of cells that can live, die, or multiply based on a few mathematical rules.

## Features

- **Interactive grid**: Users can toggle cell states by clicking.
- **Simulation control**: Start, stop, and reset the simulation using buttons.
- **Predefined patterns**: Load different predefined patterns such as Gliders and the Glider Gun.
- **Adjustable grid size**: Change the grid size using the mouse wheel.
- **Adjustable simulation speed**: Change the simulation speed using keyboard keys.

## Getting Started

### Prerequisites

- Raylib library
- C++ compiler

### Installation

1. Clone the repository:

    ```bash
    git clone https://github.com/yourusername/game-of-life.git
    cd game-of-life
    ```

2. Ensure you have the Raylib library installed. You can follow the installation instructions on the [Raylib website](https://www.raylib.com/).

3. Compile the program:

    ```bash
    g++ -o game-of-life main.cpp -lraylib -std=c++17 -lGL -lm -lpthread -ldl -lrt -lX11
    ```

4. Run the executable:

    ```bash
    ./game-of-life
    ```

### Controls

- **Mouse Left Click**: Toggle cell state.
- **Mouse Wheel**: Adjust grid size.
- **Left Arrow Key**: Decrease simulation speed.
- **Right Arrow Key**: Increase simulation speed.
- **Space Key**: Print IDs of alive cells to the console.
- **Start/Stop Button**: Start or stop the simulation.
- **Reset Button**: Reset the grid to the initial state.
- **Left/Right Button**: Navigate through predefined patterns.

## Code Structure

### Constants and Global Variables

- **Colors**: Colors used for dead/alive cells and their borders.
- **Dimensions**: Window dimensions and header height.
- **Grid Settings**: Minimum and maximum grid sizes.

### Structures

- **Cell**: Represents a cell in the grid.
  - `toggle()`: Toggles the alive state of the cell.
  - `draw()`: Draws the cell on the screen.
- **Pattern**: Represents a predefined pattern with a name, grid size, and a list of alive cell IDs.

### Functions

- **findCell()**: Finds a cell based on mouse coordinates.
- **printAliveIDS()**: Prints the IDs of alive cells.
- **findStartBtn()**: Checks if the start button was clicked.
- **makeMap()**: Creates the initial cell map based on the selected pattern.

### Main Loop

- Initializes the window and audio.
- Handles user input for toggling cells and interacting with buttons.
- Controls the simulation steps and updates the cell states.
- Renders the cells and UI elements.

## Patterns

The game includes several predefined patterns:

- **Empty**: An empty grid.
- **Gliders**: A grid with glider patterns.
- **GliderGun**: A grid with a glider gun pattern.
- **BlinkerPuffer1**: A grid with a blinker puffer pattern.

## Acknowledgments

- [Raylib](https://www.raylib.com/) for the graphics library.
- John Horton Conway for the original concept of the Game of Life.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

Enjoy playing with Conway's Game of Life! Feel free to contribute or suggest new features.