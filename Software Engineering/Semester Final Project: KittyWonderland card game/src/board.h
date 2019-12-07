#ifndef __BOARD__
#define __BOARD__

#include "utils.h"
#include "entity.h"
#include "game.h"

#define BOARD_SIZE 50
#define PLAYER_MOVE_RANGE 5
#define FRIEND_MOVE_RANGE 2

struct cell {
  int x;
  int y;
};

void init_board(struct game_data *);
void choose_random_move(int *, int *, struct entity *, struct game_data *);
void choose_friend_move(int *, int *, struct entity *, struct game_data *);
void move_entities(struct game_data *);
void find_reachable_cells(int, struct cell[], int *, struct cell[], int, struct game_data *);

#endif
