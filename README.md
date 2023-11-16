# Snake Game in C

## Project Overview
This project is a C implementation of the classic Snake game. It involves creating a playable snake game on a grid of characters, where the player controls a snake to eat fruits and avoid crashing into walls or itself.

### Game Representation
- The game is represented on a grid containing walls, fruits, and snakes.
- Special characters used:
  - `#` for walls.
  - ` ` (space) for empty space.
  - `*` for fruits.
  - `wasd` for the snake's tail.
  - `^<v>` for the snake's body.
  - `WASD` for the snake's head.
  - `x` for the dead snake's head.

### Gameplay Rules
- Snakes move one step in the direction of their head.
- Crashing into the body or wall results in death (`x`).
- Eating a fruit (`*`) grows the snake and spawns new fruit.

## Structs
- `game_state_t`: Stores the game state including the board and snake information.
- `snake_t`: Details of each snake including position and status.

## Tasks
1. **create_default_state**: Initialize the default game state.
2. **free_state**: Free allocated memory for the game state.
3. **print_board**: Print the game board to a file.
4. **update_state**: Update the game state per time step.
5. **load_board**: Read a game board from a stream into memory.
6. **initialize_snake**: Initialize snake structs from the game board.
7. **main**: Execute the game update loop.

## Testing and Debugging
- Run unit tests with `make run-unit-tests`.
- Use `make debug-unit-tests` for debugging with CGDB.
- Validate memory management with `make valgrind-test-free-state`.

## Running the Game
- Play the game using `make interactive-snake` followed by `./interactive-snake`.
- Control the snake with `wasd` keys.
- Adjust game speed with `-d` option or `[]` keys during gameplay.

## Acknowledgements
This project is based on the content of project1 of CS61C Great Ideas of Computer Architecture (Machine Structures) at UC Berkeley. Thanks to all contributors and testers who helped in developing and refining this game!
