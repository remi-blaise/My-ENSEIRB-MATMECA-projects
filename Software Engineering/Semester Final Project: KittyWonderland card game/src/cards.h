#ifndef __CARDS__
#define __CARDS__

#include "utils.h"
#include "card.h"
#include "board.h"
#include "game.h"

const struct card * draw_random_card();

void think(struct entity *, struct entity *, struct game_data *);
void steal(struct entity *, struct entity *, struct game_data *);
void panacea(struct entity *, struct entity *, struct game_data *);
void razor(struct entity *, struct entity *, struct game_data *);
void hell(struct entity *, struct entity *, struct game_data *);
void stone(struct entity *, struct entity *, struct game_data *);
void puppy(struct entity *, struct entity *, struct game_data *);

const struct card * HELL;

#endif
