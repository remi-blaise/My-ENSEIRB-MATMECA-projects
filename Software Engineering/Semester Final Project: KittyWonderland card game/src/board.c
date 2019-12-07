#include "board.h"

void init_board(struct game_data * game_data) {
  for (int i = 0; i < BOARD_SIZE; i++) {
    for (int j = 0; j < BOARD_SIZE; j++) {
      game_data->board[i][j] = 0;
    }
  }

  for (int i = 0; i < game_data->n_players; i++) {
    int x;
    int y;
    do {
      x = randint(0, BOARD_SIZE);
      y = randint(0, BOARD_SIZE);
    } while (game_data->board[x][y]);

    game_data->entities[i]->x = x;
    game_data->entities[i]->y = y;
    game_data->board[x][y] = 1;
  }
}

/*
 * Internal function
 * Check if a given position is in an array of cells.
 */
int in(int x, int y, int * n_cells, struct cell cells[]) {
  for (int i = 0; i < *n_cells; i++) {
    if (cells[i].x == x && cells[i].y == y) {
      return 1;
    }
  }
  return 0;
}

/*
 * Fill recursively an array of all reachable cells from a set of starting cells
 * with a given maximum number of movements.
 */
void find_reachable_cells(
  int n_starting_cells, struct cell starting_cells[],
  int * n_cells, struct cell cells[],
  int range, struct game_data * game_data
) {
  if (!range) {
    return;
  }

  struct cell ending_cells[(PLAYER_MOVE_RANGE - range + 1) * 4];
  int n_ending_cells = 0;

  int x;
  int y;

  for (int i = 0; i < n_starting_cells; i++) {
    for (int way = -1; way <= 1; way += 2) {
      for (int coor = 0; coor <= 1; coor++) {
        x = starting_cells[i].x;
        y = starting_cells[i].y;
        switch (coor) {
          case 0:
          x += way;
          mod(&x, BOARD_SIZE);
          break;
          case 1:
          y += way;
          mod(&y, BOARD_SIZE);
          break;
        }

        if (!in(x, y, n_cells, cells) && !game_data->board[x][y]) {
          ending_cells[n_ending_cells].x = x;
          ending_cells[n_ending_cells].y = y;
          n_ending_cells++;
          cells[*n_cells].x = x;
          cells[*n_cells].y = y;
          (*n_cells)++;
        }
      }
    }
  }
  find_reachable_cells(n_ending_cells, ending_cells, n_cells, cells, range - 1, game_data);
}

// Choose move of the entity
void choose_random_move(
  int * x, int * y, // Returned values: chosen coor
  struct entity * entity,
  struct game_data * game_data
) {
  struct cell cells[2 * PLAYER_MOVE_RANGE * (PLAYER_MOVE_RANGE + 1)];
  int n_cells = 0;

  struct cell starting_cells[1] = {{ entity->x, entity->y }};
  int n_starting_cells = 1;
  find_reachable_cells(n_starting_cells, starting_cells, &n_cells, cells, PLAYER_MOVE_RANGE, game_data);

  if (!n_cells) {
    *x = -1;
    *y = -1;
    return;
  }

  int i = randint(0, n_cells);
  *x = cells[i].x;
  *y = cells[i].y;
}

int one_axe_distance(int a, int b) {
  if (a > b) {
    return one_axe_distance(b, a);
  }

  return min(b - a, 50 - b + a);
}

int distance(struct cell c1, struct cell c2) {
  return one_axe_distance(c1.x, c2.x) + one_axe_distance(c1.y, c2.y);
}

void choose_friend_move(int * x, int * y, struct entity * entity, struct game_data * game_data) {
  struct cell cells[2 * FRIEND_MOVE_RANGE * (FRIEND_MOVE_RANGE + 1)];
  int n_cells = 0;

  struct cell starting_cells[1] = {{ entity->x, entity->y }};
  int n_starting_cells = 1;
  find_reachable_cells(n_starting_cells, starting_cells, &n_cells, cells, FRIEND_MOVE_RANGE, game_data);

  if (!n_cells) {
    *x = -1;
    *y = -1;
    return;
  }

  int min = BOARD_SIZE + 1;
  int cell_index = -1;
  struct cell target = { entity->target->x, entity->target->y };
  for (int i = 0; i < n_cells; i++) {
    int d = distance(cells[i], target);
    if (d < min) {
      min = d;
      cell_index = i;
    }
  }
  *x = cells[cell_index].x;
  *y = cells[cell_index].y;
}

void move_entities(struct game_data * game_data) {
  struct cell cells[game_data->n_players + game_data->n_friends];
  for (int i = 0; i < game_data->n_players + game_data->n_friends; i++) {
    if (
      game_data->entities[i]->type == FRIEND_TYPE && (
        game_data->entities[i]->mana >= game_data->entities[i]->card->cost ||
        game_data->entities[i]->mana_regen == 0 // skip friend turn if it has no target alive
      )
    ) {
      cells[i].x = game_data->entities[i]->x;
      cells[i].y = game_data->entities[i]->y;
      continue;
    }

    if (game_data->entities[i]->type == PLAYER_TYPE) {
      choose_random_move(&cells[i].x, &cells[i].y, game_data->entities[i], game_data);
    } else if (game_data->entities[i]->type == FRIEND_TYPE) {
      choose_friend_move(&cells[i].x, &cells[i].y, game_data->entities[i], game_data);
    }


    if (cells[i].x == -1) {
      die(i, game_data);
      // die() modify player indexes, thus we have to modify cell indexes accordingly
      cells[i].x = cells[game_data->n_players + game_data->n_friends].x;
      cells[i].y = cells[game_data->n_players + game_data->n_friends].y;
      i--;
    }
  }

  for (int i = 0; i < game_data->n_players + game_data->n_friends; i++) {
    for (int j = 0; j < game_data->n_players + game_data->n_friends; j++) {
      if (i == j) {
        continue;
      }

      if (cells[j].x == cells[i].x && cells[j].y == cells[i].y) {
        continue;
      }

      game_data->board[game_data->entities[i]->x][game_data->entities[i]->y] = 0;
      game_data->board[cells[i].x][cells[i].y] = 1;
      game_data->entities[i]->x = cells[i].x;
      game_data->entities[i]->y = cells[i].y;
    }
  }
}
