#ifndef __ENTITY__
#define __ENTITY__

#include <stdio.h>
#include "utils.h"
#include "deck.h"
#include "cards.h"
#include "game.h"

#define HAND_SIZE 5

#define PLAYER_TYPE 0
#define FRIEND_TYPE 1

struct entity {
  char type; // player or friend
  int life;
  int mana;
  int mana_regen;
  char name[64];

  int x;
  int y;

  // player attributes
  const struct card * hand[HAND_SIZE];
  struct deck * deck;

  // friend attributes
  struct entity * target;
  const struct card * card;
  int lifespan;
};

void display_player(struct entity *);
void display_puppy(struct entity *);
void display_entities(struct game_data *);
void deal_hand(struct entity *);
void shuffle_hand(struct entity *);
int select_card(struct entity *);

#endif
