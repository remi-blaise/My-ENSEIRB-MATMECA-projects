#ifndef __ASSERT__
#define __ASSERT__

#include <stdio.h>
#include <stdlib.h>

#include "../src/entity.h"
#include "../src/game.h"

void assert(int bool, char *);
void set_up_one_player_with_hand_and_deck(struct entity *);
void tear_down_one_player_with_hand_and_deck(struct entity *);
void set_up_many_players(struct game_data *);
void tear_down_many_players(struct game_data *);

const struct card * TEST;
const struct card * TEST2;

#endif
