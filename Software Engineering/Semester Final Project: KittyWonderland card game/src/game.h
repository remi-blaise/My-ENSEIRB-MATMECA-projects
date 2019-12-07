#ifndef __GAME__
#define __GAME__

#include <unistd.h>

#define BOARD_SIZE 50

struct entity;

struct game_data {
    struct entity * entities[BOARD_SIZE * BOARD_SIZE];
    int n_players;
    int n_friends;
    char board[BOARD_SIZE][BOARD_SIZE];
};

#include "card.h"
#include "cards.h"
#include "entity.h"
#include "board.h"

void apply_card(struct entity *, int, struct game_data *);
void play_turn(struct entity *, struct game_data *);
void die(int, struct game_data *);
void purge_the_dead(struct game_data *);
void generate_mana(struct game_data *);
void decay_friends(struct game_data *);
void play_game(int);

#endif
