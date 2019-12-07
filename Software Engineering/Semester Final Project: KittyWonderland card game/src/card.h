#ifndef __CARD__
#define __CARD__

#include "game.h"

struct entity;

struct card {
  char name[64];
  void (* apply) (struct entity *, struct entity *, struct game_data *);
  int cost;
};

#include "entity.h"

#endif
