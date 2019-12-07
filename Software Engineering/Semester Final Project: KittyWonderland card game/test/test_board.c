#include "../src/board.h"
#include "../src/entity.h"
#include "assert.h"

void test_init_board() {
  struct game_data _game_data;
  struct game_data * game_data = &_game_data;
  game_data->n_players = 4;

  set_up_many_players(game_data);
  init_board(game_data);
  int count_ones = 0;
  for (int i = 0; i < BOARD_SIZE; i++) {
    for (int j = 0; j < BOARD_SIZE; j++) {
      count_ones += game_data->board[i][j];
    }
  }
  assert(count_ones == game_data->n_players, "The number of 1 in board is not equal to the number of players.");
  tear_down_many_players(game_data);
}

void test_choose_move()
{
  struct game_data _game_data;
  struct game_data * game_data = &_game_data;
  game_data->n_players = 1;

  set_up_many_players(game_data);
  init_board(game_data);
  int x0 = game_data->entities[0]->x;
  int y0 = game_data->entities[0]->y;
  int x; int y;
  choose_random_move(&x, &y, game_data->entities[0], game_data);
  assert(x != x0 || y != y0, "choose_move should not return player's coor.");
  tear_down_many_players(game_data);

  set_up_many_players(game_data);
  init_board(game_data);
  for (int i = 0; i < BOARD_SIZE; i++) {
    for (int j = 0; j < BOARD_SIZE; j++) {
      game_data->board[i][j] = 1;
    }
  }
  choose_random_move(&x, &y, game_data->entities[0], game_data);
  assert(x == -1 && y == -1, "choose_move should return (-1,-1) if player cannot move.");
  tear_down_many_players(game_data);
}

void test_move_entities()
{
  struct game_data _game_data;
  struct game_data * game_data = &_game_data;
  game_data->n_players = 2;
  game_data->n_friends = 0;
  set_up_many_players(game_data);
  for (int i = 0; i < BOARD_SIZE; i++) {
    for (int j = 0; j < BOARD_SIZE; j++) {
      game_data->board[i][j] = 1;
    }
  }
  game_data->entities[0]->x = 0; game_data->entities[0]->y = 0; game_data->entities[0]->type = PLAYER_TYPE;
  game_data->entities[1]->x = 0; game_data->entities[1]->y = 2; game_data->entities[1]->type = PLAYER_TYPE;
  game_data->board[0][1] = 0;
  move_entities(game_data);
  assert(
    game_data->entities[0]->x == 0 && game_data->entities[0]->y == 0 && game_data->entities[1]->x == 0 && game_data->entities[1]->y == 2,
    "move_entities() should not make players move if they want to go to the same cell."
  );
  tear_down_many_players(game_data);
}

int main() {
  test_init_board();
  test_choose_move();
  test_move_entities();

  return 0;
}
