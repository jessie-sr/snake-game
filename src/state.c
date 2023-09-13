#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_body(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t* state, unsigned int snum);
static char next_square(game_state_t* state, unsigned int snum);
static void update_tail(game_state_t* state, unsigned int snum);
static void update_head(game_state_t* state, unsigned int snum);

/* Task 1 */
game_state_t* create_default_state() {
  // TODO: Implement this function.
  // Allocate memory for the default state.
  game_state_t* default_state = (game_state_t *)malloc(sizeof(game_state_t));
  if (default_state == NULL) {
      perror("Failed to allocate memory");
      exit(EXIT_FAILURE);
  }  

  // Allocate memory for the snakes.
  default_state->num_snakes = 1;
  default_state->snakes = (snake_t *)malloc(default_state->num_snakes * sizeof(snake_t));
  if (default_state->snakes == NULL) {
      perror("Failed to allocate memory");
      free(default_state);
      exit(EXIT_FAILURE);
  }

  default_state->snakes[0].tail_row = 2;
  default_state->snakes[0].tail_col = 2;
  default_state->snakes[0].head_row = 2;
  default_state->snakes[0].head_col = 4;
  default_state->snakes[0].live = true;

  default_state->num_rows = 18;

  // Allocate memory for the board.
  default_state->board = (char **)malloc(default_state->num_rows * sizeof(char *));
  if (default_state->board == NULL) {
      perror("Failed to allocate memory");
      free(default_state);
      exit(EXIT_FAILURE);
  }

  for (int r = 0; r < default_state->num_rows; r++) {
      default_state->board[r] = (char *)malloc(21 * sizeof(char));

      if (default_state->board[r] == NULL) {
          perror("Failed to allocate memory");
          free(default_state->board);
          free(default_state);
          exit(EXIT_FAILURE);
      }

      for (int c = 0; c < 20; c++) {
          if (r == 0 || r == default_state->num_rows - 1 || c == 0 || c == 19) {
              default_state->board[r][c] = '#';
          } else if (r == 2 && c == 2) {
              default_state->board[r][c] = 'd';
          } else if (r == 2 && c == 3) {
              default_state->board[r][c] = '>';
          } else if (r == 2 && c == 4) {
              default_state->board[r][c] = 'D';
          } else if (r == 2 && c == 9) {
              default_state->board[r][c] = '*';
          } else {
              default_state->board[r][c] = ' ';
          }
      }
      default_state->board[r][20] = '\0';
  }

  return default_state;
}

/* Task 2 */
void free_state(game_state_t* state) {
  // TODO: Implement this function.
  free(state->board);
  free(state->snakes);
  free(state);
  return;
}

/* Task 3 */
void print_board(game_state_t* state, FILE* fp) {
  // TODO: Implement this function.
  for (int r = 0; r < state->num_rows; r++) {
      int c = 0;
      while (state->board[r][c] != '\0') {
          fprintf(fp, "%c", state->board[r][c]);
          c++;
      }
      fprintf(fp, "%c", '\n');
  }
  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  return c == 'w' || c == 'a' || c == 's' || c == 'd';
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  return c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x';
}

static bool is_body(char c) {
  return c == '^' || c == '<' || c == 'v' || c == '>';
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  return is_tail(c) || is_head(c) || is_body(c);
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  if (c == '^') {
      return 'w';
  } else if (c == '>') {
      return 'd';
  } else if (c == 'v') {
      return 's';
  } else if (c == '<') {
      return 'a';
  }
  return '?';
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {    
  if (c == 'W') {
      return '^';
  } else if (c == 'D') {
      return '>';
  } else if (c == 'S') {
      return 'v';
  } else if (c == 'A') {
      return '<';
  }
  return '?';
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  if (c == 'v' || c == 's' || c == 'S') {
      return cur_row + 1;
  } else if (c == '^' || c == 'w' || c == 'W') {
      return cur_row - 1;
  } else {
      return cur_row;
  }
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  if (c == '>' || c == 'd' || c == 'D') {
      return cur_col + 1;
  } else if (c == '<' || c == 'a' || c == 'A') {
      return cur_col - 1;
  } else {
      return cur_col;
  }
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t snake = state->snakes[snum];
  char s_head = get_board_at(state, snake.head_row, snake.head_col);
  if (s_head == 'W') {
      return get_board_at(state, snake.head_row - 1, snake.head_col);
  } else if (s_head == 'D') {
      return get_board_at(state, snake.head_row, snake.head_col + 1);
  } else if (s_head == 'S') {
      return get_board_at(state, snake.head_row + 1, snake.head_col);
  } else {
      return get_board_at(state, snake.head_row, snake.head_col - 1);
  }
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t snake = state->snakes[snum];
  char s_head = get_board_at(state, snake.head_row, snake.head_col);
  if (s_head == 'W') {
      set_board_at(state, snake.head_row - 1, snake.head_col, 'W');
      set_board_at(state, snake.head_row, snake.head_col, head_to_body('W'));
      state->snakes[snum].head_row -= 1;
  } else if (s_head == 'D') {
      set_board_at(state, snake.head_row, snake.head_col + 1, 'D');
      set_board_at(state, snake.head_row, snake.head_col, head_to_body('D'));
      state->snakes[snum].head_col += 1;
  } else if (s_head == 'S') {
      set_board_at(state, snake.head_row + 1, snake.head_col, 'S');
      set_board_at(state, snake.head_row, snake.head_col, head_to_body('S'));
      state->snakes[snum].head_row += 1;
  } else {
      set_board_at(state, snake.head_row, snake.head_col - 1, 'A');
      set_board_at(state, snake.head_row, snake.head_col, head_to_body('A'));
      state->snakes[snum].head_col -= 1;
  }
  return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t snake = state->snakes[snum];
  char s_tail = get_board_at(state, snake.tail_row, snake.tail_col);
  if (s_tail == 'w') {
      char b = get_board_at(state, snake.tail_row - 1, snake.tail_col);
      set_board_at(state, snake.tail_row - 1, snake.tail_col, body_to_tail(b));
      set_board_at(state, snake.tail_row, snake.tail_col, ' ');
      state->snakes[snum].tail_row -= 1;
  } else if (s_tail == 'd') {
      char b = get_board_at(state, snake.tail_row, snake.tail_col + 1);
      set_board_at(state, snake.tail_row, snake.tail_col + 1, body_to_tail(b));
      set_board_at(state, snake.tail_row, snake.tail_col, ' ');
      state->snakes[snum].tail_col += 1;
  } else if (s_tail == 's') {
      char b = get_board_at(state, snake.tail_row + 1, snake.tail_col);
      set_board_at(state, snake.tail_row + 1, snake.tail_col, body_to_tail(b));
      set_board_at(state, snake.tail_row, snake.tail_col, ' ');
      state->snakes[snum].tail_row += 1;
  } else {
      char b = get_board_at(state, snake.tail_row, snake.tail_col - 1);
      set_board_at(state, snake.tail_row, snake.tail_col - 1, body_to_tail(b));
      set_board_at(state, snake.tail_row, snake.tail_col, ' ');
      state->snakes[snum].tail_col -= 1;
  }
  return;
}

/* Task 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
  // TODO: Implement this function.
  for (int i = 0; i < state->num_snakes; i++) {
      char next = next_square(state, i);
      if (next == '#' || is_snake(next)) {
          set_board_at(state, state->snakes[i].head_row, state->snakes[i].head_col, 'x');
          state->snakes[i].live = false;
      } else if (next == '*') {
          update_head(state, i);
          add_food(state);
      } else {
          update_tail(state, i);
          update_head(state, i);
      }
  }
  return;
}

/* Task 5 */
game_state_t* load_board(FILE* fp) {
  // TODO: Implement this function.
  game_state_t* state = malloc(sizeof(game_state_t));
  if (state == NULL) {
      perror("Failed to allocate memory");
      exit(EXIT_FAILURE);
  }  
  
  // Initializing game state
  state->num_rows = 0;
  state->board = malloc(sizeof(char*));
  state->num_snakes = 0;
  state->snakes = malloc(sizeof(char));
    
  char ch;
  int rows = 0;
  int cols = 0;
  char* current_row = NULL;

  while ((ch = fgetc(fp)) != EOF) {
      // If it's a new line or the end of the file, we process the row
      if (ch == '\n' || ch == EOF) {
          if (current_row) { // Check if there's any content to add
              state->board = realloc(state->board, sizeof(char*) * (rows + 1));
              if (state->board == NULL) {
                  perror("Failed to allocate memory");
                  exit(EXIT_FAILURE);
  }  
              state->board[rows] = current_row;
              current_row[cols] = '\0'; // null-terminate the row
              rows++;
              cols = 0;
              current_row = NULL;
          }
      } else {
          current_row = realloc(current_row, cols + 2); // +2: one for the new character, one for the null-terminator
          if (current_row == NULL) {
              perror("Failed to allocate memory"); 
              exit(EXIT_FAILURE);
  }  
          current_row[cols] = ch;
          current_row[cols + 1] = '\0';
          cols++;
      }
    }

    state->num_rows = rows;
    return state;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t snake = state->snakes[snum];
  int r = snake.tail_row;
  int c = snake.tail_col;
  char tail = state->board[r][c];
  char head = tail;

  while (!is_head(head)) {
      r = get_next_row(r, head);
      c = get_next_col(c, head);
      head = state->board[r][c];
  }

  state->snakes[snum].head_row = r;
  state->snakes[snum].head_col = c;

  return;
}

/* Task 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  // TODO: Implement this function.
  int count = 0;
  state->snakes = malloc(sizeof(snake_t));
  
  for (int r = 0; r < state->num_rows; r++) {
      int c = 0;
      while (state->board[r][c] != '\0') {
          if (is_tail(state->board[r][c])) {
              count++;
              state->snakes = (snake_t *)realloc(state->snakes, count * sizeof(snake_t));
              if (state->snakes == NULL) {
                  perror("Failed to allocate memory");
                  exit(EXIT_FAILURE);
              }

              int snum = count - 1;
              state->snakes[snum].tail_row = r;
              state->snakes[snum].tail_col = c;
              state->snakes[snum].live = true;
              find_head(state, snum);
          }
          c++;
      }
  }

  state->num_snakes = count;
  return state;
}
